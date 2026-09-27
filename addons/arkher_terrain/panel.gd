@tool
extends Control
## Painel do editor Arkher Terrain (dock).
##
## Mostra as ferramentas de escultura quando um nodo ArkherTerrain está
## selecionado. A interação acontece no viewport 3D do editor: clique
## esquerdo (sem apertar em gizmo) e arraste.

var editor_interface: EditorInterface = null
var terrain: ArkherTerrain = null
var view_3d: SubViewport = null
var _view_connected: bool = false

var _tool: int = ArkherTerrain.TOOL_RAISE
var _falloff: float = 0.0
var _dragging: bool = false
var _last_tick: int = 0

var _status: Label
var _terrain_panel: VBoxContainer
var _radius_spin: SpinBox
var _strength_spin: SpinBox
var _falloff_opt: OptionButton
var _show_brush_btn: CheckButton
var _size_spin: SpinBox
var _height_spin: SpinBox
var _res_spin: SpinBox
var _seed_spin: SpinBox
var _freq_spin: SpinBox
var _oct_spin: SpinBox
var _part_spin: SpinBox
var _steps_spin: SpinBox
var _uv_spin: SpinBox
var _mat_buttons: Array = []
var _rough_spins: Array = []
var _metal_spins: Array = []
var _file_dialog: FileDialog
var _tool_buttons: Array = []
var _material_slot: int = -1
var _undo_heights: PackedFloat32Array = PackedFloat32Array()
var _undo_weights: PackedFloat32Array = PackedFloat32Array()

func _ready() -> void:
	_build_ui()

func _process(_delta: float) -> void:
	if not _view_connected and editor_interface != null:
		_try_connect_view()

func _try_connect_view() -> void:
	if view_3d != null and is_instance_valid(view_3d):
		return
	view_3d = editor_interface.get_editor_viewport_3d()
	if view_3d == null:
		return
	view_3d.gui_input.connect(_on_3d_gui_input)
	_view_connected = true

# ---------------------------------------------------------------------- ui
func _section(title: String, parent: Control) -> VBoxContainer:
	var box := VBoxContainer.new()
	box.add_theme_constant_override("separation", 4)
	var label := Label.new()
	label.text = title
	label.add_theme_font_size_override("font_size", 12)
	label.tooltip_text = title
	parent.add_child(box)
	box.add_child(label)
	return box

func _row(label_text: String, control: Control, tip: String = "") -> HBoxContainer:
	var row := HBoxContainer.new()
	row.add_theme_constant_override("separation", 4)
	var l := Label.new()
	l.text = label_text
	l.custom_minimum_size.x = 88
	row.add_child(l)
	row.add_child(control)
	if tip != "":
		row.tooltip_text = tip
	return row

func _spin(minv: float, maxv: float, value: float, step: float = 0.1) -> SpinBox:
	var s := SpinBox.new()
	s.min_value = minv
	s.max_value = maxv
	s.value = value
	s.step = step
	s.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	return s

func _build_ui() -> void:
	var margin := MarginContainer.new()
	margin.add_theme_constant_override("margin_left", 8)
	margin.add_theme_constant_override("margin_right", 8)
	margin.add_theme_constant_override("margin_top", 8)
	margin.add_theme_constant_override("margin_bottom", 8)
	margin.set_anchors_preset(Control.PRESET_FULL_RECT)
	add_child(margin)
	var root := VBoxContainer.new()
	root.add_theme_constant_override("separation", 8)
	margin.add_child(root)

	_status = Label.new()
	_status.text = "Nenhum ArkherTerrain selecionado."
	_status.tooltip_text = "Clique no nodo ArkherTerrain na Scene Tree para editar."
	_status.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	root.add_child(_status)

	var tools_box := _section("Ferramentas — arraste com botão ESQUERDO no viewport 3D (clique em área vazia perto do terreno)", root)
	var group := ButtonGroup.new()
	var tool_names := ["Levantar", "Abaixar", "Suavizar", "Achatar", "Pint. A", "Pint. B", "Pint. C", "Pint. D"]
	for i in tool_names.size():
		var b := Button.new()
		b.text = tool_names[i]
		b.toggle_mode = true
		b.button_group = group
		b.pressed.connect(_on_tool_pressed.bind(i))
		tools_box.add_child(b)
		_tool_buttons.append(b)
	_tool_buttons[0].pressed = true  # "Levantar" ativo por padrão

	var brush_box := _section("Pincel", root)
	_radius_spin = _spin(0.5, 200.0, 6.0, 0.5)
	_radius_spin.value_changed.connect(_on_radius_changed)
	brush_box.add_child(_row("Raio (m)", _radius_spin, "Raio do pincel em metros."))
	_strength_spin = _spin(0.1, 10.0, 1.0, 0.1)
	brush_box.add_child(_row("Força", _strength_spin, "Velocidade do gesto (m/s)."))
	_falloff_opt = OptionButton.new()
	_falloff_opt.add_item("Suave", 0)
	_falloff_opt.add_item("Linear", 1)
	_falloff_opt.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	_falloff_opt.item_selected.connect(_on_falloff_changed)
	brush_box.add_child(_row("Decaimento", _falloff_opt, "Forma da falloff do pincel."))
	_show_brush_btn = CheckButton.new()
	_show_brush_btn.text = "Mostrar pincel"
	_show_brush_btn.button_pressed = true
	_show_brush_btn.toggled.connect(_on_show_brush_toggled)
	brush_box.add_child(_show_brush_btn)

	# Seção visível apenas com terreno selecionado.
	_terrain_panel = VBoxContainer.new()
	_terrain_panel.add_theme_constant_override("separation", 8)
	_terrain_panel.visible = false
	root.add_child(_terrain_panel)

	var map_box := _section("Terreno", _terrain_panel)
	_size_spin = _spin(8.0, 4096.0, 128.0, 8.0)
	_size_spin.value_changed.connect(_on_size_changed)
	map_box.add_child(_row("Tamanho (m)", _size_spin, "Largura e profundidade do mapa (m)."))
	_height_spin = _spin(0.0, 500.0, 24.0, 1.0)
	_height_spin.value_changed.connect(_on_height_changed)
	map_box.add_child(_row("Altura máx (m)", _height_spin, "Altura total do mapa (m)."))
	_res_spin = _spin(3.0, 1025.0, 129.0, 2.0)
	_res_spin.value_changed.connect(_on_resolution_changed)
	map_box.add_child(_row("Resolução", _res_spin, "Vértices por lado (ímpar). Mantém a forma atual."))
	_uv_spin = _spin(0.01, 128.0, 16.0, 0.5)
	_uv_spin.value_changed.connect(_on_uv_changed)
	map_box.add_child(_row("Escala UV", _uv_spin, "Repetição das texturas de material."))

	var gen_box := _section("Gerar / Erosão", _terrain_panel)
	_seed_spin = _spin(0.0, 99999.0, 0.0, 1.0)
	gen_box.add_child(_row("Seed", _seed_spin))
	_freq_spin = _spin(0.001, 0.5, 0.008, 0.001)
	gen_box.add_child(_row("Frequência", _freq_spin, "Escala do ruído (menor = montanhas maiores)."))
	_oct_spin = _spin(1.0, 8.0, 4.0, 1.0)
	gen_box.add_child(_row("Octavas", _oct_spin))
	gen_box.add_child(_button("Gerar (ruído FBM)", "Reescreve o relevo com ruído procedural."))
	_part_spin = _spin(100.0, 20000.0, 3000.0, 100.0)
	gen_box.add_child(_row("Partículas", _part_spin, "Quantidade de partículas de erosão."))
	_steps_spin = _spin(4.0, 128.0, 24.0, 1.0)
	gen_box.add_child(_row("Passos", _steps_spin, "Passos de fluxo por partícula."))
	gen_box.add_child(_button("Erodir (hidráulica)", "Água escava vales e deposita sedimento."))

	var mat_box := _section("Materiais — A=R, B=G, C=B, D=A (canais do splat)", _terrain_panel)
	var letters := ["A", "B", "C", "D"]
	for i in 4:
		var b := Button.new()
		b.text = "Material %s: —" % letters[i]
		b.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		b.pressed.connect(_on_material_pick.bind(i))
		mat_box.add_child(b)
		_mat_buttons.append(b)
		var r := _spin(0.0, 1.0, 0.9, 0.05)
		r.value_changed.connect(_on_rough_changed.bind(i))
		var mt := _spin(0.0, 1.0, 0.0, 0.05)
		mt.value_changed.connect(_on_metal_changed.bind(i))
		var props_row := HBoxContainer.new()
		var rl := Label.new()
		rl.text = "Rough"
		rl.custom_minimum_size.x = 40
		var ml := Label.new()
		ml.text = "Metal"
		ml.custom_minimum_size.x = 40
		props_row.add_child(rl)
		props_row.add_child(r)
		props_row.add_child(ml)
		props_row.add_child(mt)
		mat_box.add_child(props_row)
		_rough_spins.append(r)
		_metal_spins.append(mt)

	var save_box := _section("Dados", _terrain_panel)
	save_box.add_child(_button("Salvar dados (PNG16)", "Salva heightmap + pesos na pasta da cena."))
	save_box.add_child(_button("Carregar dados", "Relê os dados salvos."))

	_file_dialog = FileDialog.new()
	_file_dialog.file_mode = FileDialog.FILE_MODE_OPEN_FILE
	_file_dialog.access = FileDialog.ACCESS_RESOURCES
	_file_dialog.add_filter("*.png ; Textura PNG")
	_file_dialog.add_filter("*.jpg, *.jpeg ; Textura JPG")
	_file_dialog.file_selected.connect(_on_material_file_selected)
	add_child(_file_dialog)

func _button(text: String, tip: String = "") -> Button:
	var b := Button.new()
	b.text = text
	if tip != "":
		b.tooltip_text = tip
	b.pressed.connect(_on_generic_button.bind(b))
	return b

# ------------------------------------------------------------- selection
func terrain_changed(t: Node) -> void:
	terrain = t as ArkherTerrain
	if terrain == null:
		_status.text = "Nenhum ArkherTerrain selecionado."
		_terrain_panel.visible = false
		hide_brush_soft()
		return
	_terrain_panel.visible = true
	_status.text = "Editando: %s  (grade %dx%d)" % [String(terrain.name), terrain.grid_size(), terrain.grid_size()]
	_radius_spin.value = terrain.brush_radius
	_size_spin.value = terrain.size.x
	_height_spin.value = terrain.height_scale
	_res_spin.value = terrain.resolution
	_uv_spin.value = terrain.uv_scale
	var letters := ["A", "B", "C", "D"]
	for i in 4:
		var tex: Texture2D = terrain.get_material_albedo(i)
		_mat_buttons[i].text = "Material %s: %s" % [letters[i], "—" if tex == null else String(tex.get_path()).get_file()]
		_rough_spins[i].value = terrain.get_material_roughness(i)
		_metal_spins[i].value = terrain.get_material_metallic(i)
	_show_brush_btn.button_pressed = terrain.brush_visible

func hide_brush_soft() -> void:
	if terrain and terrain.has_method("hide_brush"):
		terrain.hide_brush()

# --------------------------------------------------------------- callbacks
func _on_tool_pressed(i: int) -> void:
	_tool = i + 1  # índice do botão → TOOL_RAISE(1) ... TOOL_PAINT_D(8)

func _on_radius_changed(v: float) -> void:
	if terrain:
		terrain.brush_radius = v

func _on_falloff_changed(i: int) -> void:
	_falloff = float(i)

func _on_show_brush_toggled(v: bool) -> void:
	if terrain:
		terrain.brush_visible = v

func _on_size_changed(v: float) -> void:
	if terrain:
		terrain.size = Vector2(v, v)

func _on_height_changed(v: float) -> void:
	if terrain:
		terrain.height_scale = v

func _on_resolution_changed(v: float) -> void:
	if terrain:
		terrain.resolution = int(v)

func _on_uv_changed(v: float) -> void:
	if terrain:
		terrain.uv_scale = v

func _on_rough_changed(v: float, i: int) -> void:
	if terrain:
		terrain.set("roughness_" + "abcd"[i], v)

func _on_metal_changed(v: float, i: int) -> void:
	if terrain:
		terrain.set("metallic_" + "abcd"[i], v)

func _on_material_pick(i: int) -> void:
	_material_slot = i
	_file_dialog.popup_centered(Vector2i(640, 480))

func _on_material_file_selected(path: String) -> void:
	if terrain == null or _material_slot < 0:
		return
	var tex: Texture2D = load(path)
	if tex == null:
		_status.text = "Falha ao carregar textura: " + path
		return
	terrain.set("albedo_" + "abcd"[_material_slot], tex)
	_mat_buttons[_material_slot].text = "Material %s: %s" % ["ABCD"[_material_slot], path.get_file()]
	_status.text = "Material %s = %s" % ["ABCD"[_material_slot], path.get_file()]

func _on_generic_button(b: Button) -> void:
	if terrain == null:
		return
	match b.text:
		"Gerar (ruído FBM)":
			terrain.generate_noise(int(_seed_spin.value), _freq_spin.value, int(_oct_spin.value), 0.5, 2.1)
			_status.text = "Relevo gerado (seed %d)." % int(_seed_spin.value)
		"Erodir (hidráulica)":
			terrain.erode_hydraulic(int(_part_spin.value), int(_steps_spin.value), 1.0, 1.0)
			_status.text = "Erosão aplicada (%d partículas)." % int(_part_spin.value)
		"Salvar dados (PNG16)":
			if terrain.save_to_dir(terrain.get_data_dir()):
				_status.text = "Salvo em: " + terrain.get_data_dir()
			else:
				_status.text = "Falha ao salvar em: " + terrain.get_data_dir()
		"Carregar dados":
			if terrain.load_from_dir(terrain.get_data_dir()):
				_status.text = "Carregado de: " + terrain.get_data_dir()
			else:
				_status.text = "Nenhum dado salvo em: " + terrain.get_data_dir()

# -------------------------------------------------------------- 3D picking
func _pick_surface(screen_pos: Vector2) -> Vector3:
	var cam: Camera3D = view_3d.get_camera_3d()
	if cam == null:
		return Vector3.INF
	var origin := cam.project_ray_origin(screen_pos)
	var dir := cam.project_ray_normal(screen_pos)
	var t := 0.25
	var tstep := 0.25
	var prev_t := 0.0
	var prev_s := 0
	var i := 0
	while i < 1200:
		var p := origin + dir * t
		var h := terrain.height_at_world(p.x, p.z)
		var s := 0
		if not is_inf(h):
			s = 1 if p.y > h else -1
			if prev_s != 0 and s != prev_s:
				# Bissecção entre o último acerto e agora.
				var a := prev_t
				var b := t
				for _k in 24:
					var m := (a + b) * 0.5
					var pm := origin + dir * m
					var hm := terrain.height_at_world(pm.x, pm.z)
					if is_inf(hm) or pm.y < hm:
						b = m
					else:
						a = m
				var hp := origin + dir * ((a + b) * 0.5)
				return Vector3(hp.x, terrain.height_at_world(hp.x, hp.z), hp.z)
			prev_t = t
			prev_s = s
		t += tstep
		tstep = minf(tstep * 1.15, 6.0)
		i += 1
	return Vector3.INF

func _on_3d_gui_input(event: InputEvent) -> void:
	if terrain == null or view_3d == null:
		return
	if event is InputEventMouseButton:
		var mb := event as InputEventMouseButton
		if mb.button_index == MOUSE_BUTTON_LEFT:
			if mb.pressed:
				_dragging = true
				_last_tick = Time.get_ticks_msec()
				_undo_heights = terrain.snapshot_heights()
				_undo_weights = terrain.snapshot_weights()
				if _tool == ArkherTerrain.TOOL_FLATTEN:
					var p := _pick_surface(mb.position)
					if p != Vector3.INF:
						terrain.set_flat_target(terrain.height_at_world(p.x, p.z))
						terrain.set_brush_position(p)
			else:
				_dragging = false
				if _last_tick > 0:
					_last_tick = 0
					_commit_undo_if_changed()
	elif event is InputEventMouseMotion:
		var mm := event as InputEventMouseMotion
		if not _dragging:
			# Move o pincel acompanhando o mouse, sem esculpir.
			var p := _pick_surface(mm.position)
			if p == Vector3.INF:
				hide_brush_soft()
			else:
				terrain.set_brush_position(p)
			return
		var now := Time.get_ticks_msec()
		var dt := clampf(float(now - _last_tick) / 1000.0, 0.0, 0.1)
		_last_tick = now
		var p2 := _pick_surface(mm.position)
		if p2 == Vector3.INF:
			hide_brush_soft()
			return
		terrain.set_brush_position(p2)
		var amount_meters := _strength_spin.value * dt * 4.0
		terrain.sculpt(p2.x, p2.z, _tool, amount_meters, _falloff)

func _commit_undo_if_changed() -> void:
	if terrain == null or editor_interface == null:
		return
	if terrain.snapshot_heights() == _undo_heights and terrain.snapshot_weights() == _undo_weights:
		return
	var ur := editor_interface.get_undo_redo()
	ur.create_action("Arkher: escultura")
	ur.add_do_method(terrain, "apply_data", terrain.snapshot_heights(), terrain.snapshot_weights())
	ur.add_undo_method(terrain, "apply_data", _undo_heights.duplicate(), _undo_weights.duplicate())
	ur.commit_action()
