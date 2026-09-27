@tool
class_name ArkherTerrain
extends Node3D
## Terreno Arkher v0.1 — o primeiro pilar da Arkher Engine.
##
## Heightmap com escultura (levantar/abaixar/suavizar/achatar), pintura de 4
## materiais (splat por vértice), ruído FBM, erosão hidráulica e save/load
## PNG16. Funciona no editor (com o plugin do addon) e em runtime.
##
## Os dados (heightmap + pesos) ficam na pasta `arkher_terrain_<nome>/`
## ao lado da cena (ou em user:// fora de cena), com heights.png e weights.png.

signal heightmap_changed
signal weights_changed

const TOOL_NONE := 0
const TOOL_RAISE := 1
const TOOL_LOWER := 2
const TOOL_SMOOTH := 3
const TOOL_FLATTEN := 4
const TOOL_PAINT_A := 5
const TOOL_PAINT_B := 6
const TOOL_PAINT_C := 7
const TOOL_PAINT_D := 8

const _SHADER := preload("res://addons/arkher_terrain/terrain_shader.gdshader")

# ------------------------------------------------------------------- storage
var _size: Vector2 = Vector2(128.0, 128.0)
var _height_scale: float = 24.0
var _resolution: int = 129
var _uv_scale: float = 16.0
var _brush_radius: float = 6.0
var _brush_visible: bool = true
var _albedo: Array = [null, null, null, null]
var _roughness: Array = [0.95, 0.9, 0.85, 0.8]
var _metallic: Array = [0.0, 0.0, 0.0, 0.0]
var _flat_target: float = 0.0

# ------------------------------------------------------------------ exports
@export_group("Mapa")
@export var size: Vector2:
	get:
		return _size
	set(v):
		var n := Vector2(maxf(v.x, 1.0), maxf(v.y, 1.0))
		if n == _size:
			return
		_size = n
		_on_map_changed()

@export var height_scale: float:
	get:
		return _height_scale
	set(v):
		var n := maxf(v, 0.0)
		if is_equal_approx(n, _height_scale):
			return
		_height_scale = n
		_on_map_changed()

@export_range(3, 1025, 1) var resolution: int:
	get:
		return _resolution
	set(v):
		var n := clampi(v, 3, 1025)
		if n % 2 == 0:
			n += 1
		if n == _resolution:
			return
		_resolution = n
		_on_resolution_changed()

@export_group("Materiais (A=R, B=G, C=B, D=A)")
@export var albedo_a: Texture2D:
	get:
		return _albedo[0]
	set(v):
		_albedo[0] = v
		_on_material_changed()

@export var albedo_b: Texture2D:
	get:
		return _albedo[1]
	set(v):
		_albedo[1] = v
		_on_material_changed()

@export var albedo_c: Texture2D:
	get:
		return _albedo[2]
	set(v):
		_albedo[2] = v
		_on_material_changed()

@export var albedo_d: Texture2D:
	get:
		return _albedo[3]
	set(v):
		_albedo[3] = v
		_on_material_changed()

@export_range(0.0, 1.0, 0.05) var roughness_a: float:
	get:
		return _roughness[0]
	set(v):
		_roughness[0] = clampf(v, 0.0, 1.0)
		_on_material_changed()

@export_range(0.0, 1.0, 0.05) var roughness_b: float:
	get:
		return _roughness[1]
	set(v):
		_roughness[1] = clampf(v, 0.0, 1.0)
		_on_material_changed()

@export_range(0.0, 1.0, 0.05) var roughness_c: float:
	get:
		return _roughness[2]
	set(v):
		_roughness[2] = clampf(v, 0.0, 1.0)
		_on_material_changed()

@export_range(0.0, 1.0, 0.05) var roughness_d: float:
	get:
		return _roughness[3]
	set(v):
		_roughness[3] = clampf(v, 0.0, 1.0)
		_on_material_changed()

@export_range(0.0, 1.0, 0.05) var metallic_a: float:
	get:
		return _metallic[0]
	set(v):
		_metallic[0] = clampf(v, 0.0, 1.0)
		_on_material_changed()

@export_range(0.0, 1.0, 0.05) var metallic_b: float:
	get:
		return _metallic[1]
	set(v):
		_metallic[1] = clampf(v, 0.0, 1.0)
		_on_material_changed()

@export_range(0.0, 1.0, 0.05) var metallic_c: float:
	get:
		return _metallic[2]
	set(v):
		_metallic[2] = clampf(v, 0.0, 1.0)
		_on_material_changed()

@export_range(0.0, 1.0, 0.05) var metallic_d: float:
	get:
		return _metallic[3]
	set(v):
		_metallic[3] = clampf(v, 0.0, 1.0)
		_on_material_changed()

@export_range(0.01, 128.0) var uv_scale: float:
	get:
		return _uv_scale
	set(v):
		_uv_scale = clampf(v, 0.01, 128.0)
		_on_material_changed()

@export_group("Pincel")
@export var brush_radius: float:
	get:
		return _brush_radius
	set(v):
		_brush_radius = maxf(v, 0.1)
		_on_brush_changed()

@export var brush_visible: bool:
	get:
		return _brush_visible
	set(v):
		_brush_visible = v
		_on_brush_changed()

# -------------------------------------------------------------------- data
var _w: int = 0
var _heights: PackedFloat32Array  # _w * _w valores em 0..1
var _weights: PackedFloat32Array  # _w * _w * 4 pesos (A,B,C,D)
var _mesh: MeshInstance3D
var _brush: MeshInstance3D
var _material: ShaderMaterial
var _rebuild_queued: bool = false

# -------------------------------------------------------------------- setup
func _ready() -> void:
	if _mesh == null:
		_mesh = MeshInstance3D.new()
		_mesh.name = "ArkherTerrainMesh"
		add_child(_mesh)
	if _w != _resolution:
		_init_data(_resolution, false)
		var dir := get_data_dir()
		if not load_from_dir(dir):
			_default_data()
	_build_mesh()
	_setup_brush()

func _default_data() -> void:
	# Relevo suave inicial para nodos novos.
	generate_noise(0, 0.008, 4, 0.5, 2.1)

func _init_data(new_w: int, preserve: bool) -> void:
	new_w = clampi(new_w, 3, 1025)
	if new_w % 2 == 0:
		new_w += 1
	var old_w := _w
	var old_h := _heights
	var old_wt := _weights
	_w = new_w
	_heights = PackedFloat32Array()
	_heights.resize(_w * _w)
	_heights.fill(0.0)
	_weights = PackedFloat32Array()
	_weights.resize(_w * _w * 4)
	_weights.fill(0.0)
	for i in _w * _w:
		_weights[i * 4] = 1.0
	if preserve and old_w > 0 and old_w != _w:
		for y in _w:
			for x in _w:
				var ox := clampi(int(round(float(old_w - 1) * float(x) / float(_w - 1))), 0, old_w - 1)
				var oy := clampi(int(round(float(old_w - 1) * float(y) / float(_w - 1))), 0, old_w - 1)
				_heights[y * _w + x] = old_h[oy * old_w + ox]
				var oi := (oy * old_w + ox) * 4
				var ni := (y * _w + x) * 4
				_weights[ni] = old_wt[oi]
				_weights[ni + 1] = old_wt[oi + 1]
				_weights[ni + 2] = old_wt[oi + 2]
				_weights[ni + 3] = old_wt[oi + 3]

# ------------------------------------------------------------------ access
func grid_size() -> int:
	return _w

func cell_world_size() -> float:
	return _size.x / float(maxi(_w - 1, 1))

func get_data_dir() -> String:
	var s := get_scene_file_path()
	if s != "":
		return s.get_base_dir().path_join("arkher_terrain_" + String(name).to_snake_case())
	return "user://arkher_terrain_" + String(name).to_snake_case()

func get_material_albedo(slot: int) -> Texture2D:
	if slot >= 0 and slot < 4:
		return _albedo[slot]
	return null

func get_material_roughness(slot: int) -> float:
	if slot >= 0 and slot < 4:
		return _roughness[slot]
	return 0.9

func get_material_metallic(slot: int) -> float:
	if slot >= 0 and slot < 4:
		return _metallic[slot]
	return 0.0

func snapshot_heights() -> PackedFloat32Array:
	return _heights.duplicate()

func snapshot_weights() -> PackedFloat32Array:
	return _weights.duplicate()

## Aplica heightmap + pesos (usado pelo undo/redo do editor).
func apply_data(p_heights: PackedFloat32Array, p_weights: PackedFloat32Array) -> void:
	if p_heights.size() == _w * _w:
		_heights = p_heights
	if p_weights.size() == _w * _w * 4:
		_weights = p_weights
	_queue_rebuild()

func set_flat_target(world_height: float) -> void:
	_flat_target = world_height

# -------------------------------------------------------------- sampling
func _h_at(x: int, y: int) -> float:
	return _heights[clampi(y, 0, _w - 1) * _w + clampi(x, 0, _w - 1)]

func _bilinear(arr: PackedFloat32Array, u: float, v: float) -> float:
	var fx := clampf(u, 0.0, 1.0) * float(_w - 1)
	var fy := clampf(v, 0.0, 1.0) * float(_w - 1)
	var x0 := int(fx)
	var y0 := int(fy)
	var x1 := mini(x0 + 1, _w - 1)
	var y1 := mini(y0 + 1, _w - 1)
	var tx := fx - float(x0)
	var ty := fy - float(y0)
	var a := arr[y0 * _w + x0]
	var b := arr[y0 * _w + x1]
	var c := arr[y1 * _w + x0]
	var d := arr[y1 * _w + x1]
	return a + (b - a) * tx + (c - a) * ty + (a - b - c + d) * tx * ty

## Altura do terreno em coordenadas de mundo (INF fora do mapa).
func height_at_world(wx: float, wz: float) -> float:
	if _w == 0:
		return INF
	var u := (wx + _size.x * 0.5) / _size.x
	var v := (wz + _size.y * 0.5) / _size.y
	if u < 0.0 or u > 1.0 or v < 0.0 or v > 1.0:
		return INF
	return _bilinear(_heights, u, v) * _height_scale

# -------------------------------------------------------------- sculpting
func _falloff(dx: float, dy: float, falloff: float) -> float:
	var d := sqrt(dx * dx + dy * dy)
	if d >= 1.0:
		return 0.0
	var t := 1.0 - d
	if falloff > 0.5:
		return t
	return t * t * (3.0 - 2.0 * t)

## Esculve/pinta em `wx,wz` (mundo). `amount_meters` = quantidade do gesto.
func sculpt(wx: float, wz: float, tool: int, amount_meters: float, falloff: float = 0.0) -> void:
	if _w == 0 or tool == TOOL_NONE:
		return
	var cx := (wx + _size.x * 0.5) / _size.x * float(_w - 1)
	var cy := (wz + _size.y * 0.5) / _size.y * float(_w - 1)
	var r := _brush_radius / cell_world_size()
	if r < 0.01:
		return
	var x0 := maxi(int(cx - r), 0)
	var x1 := mini(int(cx + r), _w - 1)
	var y0 := maxi(int(cy - r), 0)
	var y1 := mini(int(cy + r), _w - 1)
	var hs := maxf(_height_scale, 0.001)
	if tool <= TOOL_FLATTEN:
		var target := 0.0
		if tool == TOOL_FLATTEN:
			target = clampf(_flat_target / hs, 0.0, 1.0)
		for y in range(y0, y1 + 1):
			for x in range(x0, x1 + 1):
				var f := _falloff(absf(float(x - cx)) / r, absf(float(y - cy)) / r, falloff)
				if f <= 0.001:
					continue
				var i := y * _w + x
				match tool:
					TOOL_RAISE:
						_heights[i] = clampf(_heights[i] + amount_meters * f / hs, 0.0, 1.0)
					TOOL_LOWER:
						_heights[i] = clampf(_heights[i] - amount_meters * f / hs, 0.0, 1.0)
					TOOL_SMOOTH:
						var avg := 0.0
						var n := 0
						for dz in [-1, 0, 1]:
							for dx in [-1, 0, 1]:
								avg += _heights[clampi(y + dz, 0, _w - 1) * _w + clampi(x + dx, 0, _w - 1)]
								n += 1
						_heights[i] = lerpf(_heights[i], avg / float(n), clampf(amount_meters * f / maxf(hs * 0.25, 0.02), 0.0, 1.0))
					TOOL_FLATTEN:
						_heights[i] = lerpf(_heights[i], target, clampf(amount_meters * f / maxf(hs * 0.25, 0.02), 0.0, 1.0))
		heightmap_changed.emit()
	else:
		var slot := tool - TOOL_PAINT_A
		if slot < 0 or slot > 3:
			return
		for y in range(y0, y1 + 1):
			for x in range(x0, x1 + 1):
				var f := _falloff(absf(float(x - cx)) / r, absf(float(y - cy)) / r, falloff)
				if f <= 0.001:
					continue
				var i := (y * _w + x) * 4
				var s := _weights[i] + _weights[i + 1] + _weights[i + 2] + _weights[i + 3]
				var k := clampf(amount_meters * f / 0.5, 0.0, 1.0)
				for s2 in 4:
					if s2 == slot:
						_weights[i + s2] += s * k
					else:
						_weights[i + s2] *= (1.0 - k)
		weights_changed.emit()
	_queue_rebuild()

# ------------------------------------------------------------- generation
func generate_noise(p_seed: int, p_frequency: float, p_octaves: int, p_persistence: float, p_lacunarity: float) -> void:
	var n := FastNoiseLite.new()
	n.noise_type = FastNoiseLite.TYPE_PERLIN
	n.seed = p_seed
	n.frequency = clampf(p_frequency, 0.001, 10.0)
	n.octave_count = clampi(p_octaves, 1, 8)
	n.octave_persistence = clampf(p_persistence, 0.0, 1.0)
	n.octave_lacunarity = clampf(p_lacunarity, 1.0, 4.0)
	var minv := INF
	var maxv := -INF
	for y in _w:
		for x in _w:
			var nx := (float(x) / float(_w - 1) - 0.5) * _size.x
			var nz := (float(y) / float(_w - 1) - 0.5) * _size.y
			var v := n.get_noise_2d(nx, nz)
			_heights[y * _w + x] = v
			if v < minv:
				minv = v
			if v > maxv:
				maxv = v
	var span := maxf(maxv - minv, 1e-5)
	for i in _heights.size():
		_heights[i] = clampf((_heights[i] - minv) / span, 0.0, 1.0)
	heightmap_changed.emit()
	_queue_rebuild()

## Erosão hidráulica simples: partículas de água escavam o terreno indo
## para o vizinho mais baixo e depositam sedimento.
func erode_hydraulic(p_particles: int, p_steps: int, p_erode: float, p_deposit: float) -> void:
	var rng := RandomNumberGenerator.new()
	rng.seed = rng.randi()
	for _p in p_particles:
		var x := rng.randi_range(1, _w - 2)
		var y := rng.randi_range(1, _w - 2)
		var water := 1.0
		var sediment := 0.0
		for _s in p_steps:
			var i := y * _w + x
			var h := _heights[i]
			var nh := h
			var nx := x
			var ny := y
			if _heights[i - 1] < nh:
				nh = _heights[i - 1]
				nx = x - 1
				ny = y
			if _heights[i + 1] < nh:
				nh = _heights[i + 1]
				nx = x + 1
				ny = y
			if _heights[i - _w] < nh:
				nh = _heights[i - _w]
				nx = x
				ny = y - 1
			if _heights[i + _w] < nh:
				nh = _heights[i + _w]
				nx = x
				ny = y + 1
			if nh >= h:
				break
			var slope := h - nh
			# capacity cresce com a inclinação: onde é plano, deposita.
			var capacity := slope * water * 4.0
			var move := minf(sediment, maxf(capacity - slope, 0.0) * p_deposit)
			var erode := minf(slope * water * 0.1 * p_erode, slope, water * 0.1)
			_heights[i] -= erode
			_heights[ny * _w + nx] += erode * 0.5
			sediment += erode * 0.5
			_heights[ny * _w + nx] += move
			sediment -= move
			x = nx
			y = ny
			water *= 0.99
	for i in _heights.size():
		_heights[i] = clampf(_heights[i], 0.0, 1.0)
	heightmap_changed.emit()
	_queue_rebuild()

# ------------------------------------------------------------------ saving
## Salva heightmap (16-bit em PNG) e pesos (RGBA8) em `dir`. Retorna true se ok.
func save_to_dir(dir: String) -> bool:
	if DirAccess.make_dir_recursive_absolute(dir) != OK and not DirAccess.dir_exists_absolute(dir):
		return false
	var img := Image.create(_w, _w, false, Image.FORMAT_RGBA8)
	var data := PackedByteArray()
	data.resize(_w * _w * 4)
	for i in _w * _w:
		var v := int(clampf(_heights[i], 0.0, 1.0) * 65535.0)
		var hi := v >> 8
		var lo := v & 255
		var o := i * 4
		data[o] = hi
		data[o + 1] = hi
		data[o + 2] = lo
		data[o + 3] = lo
	img.set_data(data)
	if img.save_png(dir.path_join("heights.png")) != OK:
		return false
	var img2 := Image.create(_w, _w, false, Image.FORMAT_RGBA8)
	var d2 := PackedByteArray()
	d2.resize(_w * _w * 4)
	for i in _w * _w:
		var o := i * 4
		d2[o] = clampi(int(_weights[o] * 255.0), 0, 255)
		d2[o + 1] = clampi(int(_weights[o + 1] * 255.0), 0, 255)
		d2[o + 2] = clampi(int(_weights[o + 2] * 255.0), 0, 255)
		d2[o + 3] = clampi(int(_weights[o + 3] * 255.0), 0, 255)
	img2.set_data(d2)
	return img2.save_png(dir.path_join("weights.png")) == OK

func load_from_dir(dir: String) -> bool:
	if not FileAccess.file_exists(dir.path_join("heights.png")):
		return false
	var img := Image.new()
	if img.load_png_from_file(dir.path_join("heights.png")) != OK:
		return false
	if img.get_width() != _w or img.get_height() != _w:
		return false
	var data := img.get_data()
	for i in _w * _w:
		var o := i * 4
		_heights[i] = float(data[o] * 256 + data[o + 2]) / 65535.0
	var img2 := Image.new()
	if img2.load_png_from_file(dir.path_join("weights.png")) == OK and img2.get_width() == _w and img2.get_height() == _w:
		var d2 := img2.get_data()
		for i in _w * _w:
			var o := i * 4
			_weights[o] = d2[o] / 255.0
			_weights[o + 1] = d2[o + 1] / 255.0
			_weights[o + 2] = d2[o + 2] / 255.0
			_weights[o + 3] = d2[o + 3] / 255.0
	heightmap_changed.emit()
	weights_changed.emit()
	_queue_rebuild()
	return true

# ------------------------------------------------------------------- brush
func _setup_brush() -> void:
	_brush = MeshInstance3D.new()
	_brush.name = "ArkherBrush"
	var sm := SphereMesh.new()
	sm.radius = 1.0
	sm.height = 2.0
	sm.rings = 12
	sm.radial_segments = 24
	var mat := StandardMaterial3D.new()
	mat.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
	mat.albedo_color = Color(0.25, 0.65, 1.0, 0.22)
	mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
	mat.cull_mode = BaseMaterial3D.CULL_DISABLED
	mat.depth_draw = BaseMaterial3D.DEPTH_DRAW_TRANSPARENT
	sm.material = mat
	_brush.mesh = sm
	_brush.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	add_child(_brush)
	_brush.scale = Vector3.ONE * _brush_radius
	_brush.visible = _brush_visible and Engine.is_editor_hint()

func set_brush_position(world: Vector3) -> void:
	if _brush == null:
		return
	_brush.visible = _brush_visible and Engine.is_editor_hint()
	_brush.scale = Vector3.ONE * _brush_radius
	_brush.position = Vector3(world.x, height_at_world(world.x, world.z), world.z)

func hide_brush() -> void:
	if _brush:
		_brush.visible = false

func _on_brush_changed() -> void:
	if is_inside_tree() and _brush:
		_brush.visible = _brush_visible and Engine.is_editor_hint()

# ------------------------------------------------------------------- mesh
func _on_map_changed() -> void:
	if is_inside_tree():
		_queue_rebuild()

func _on_resolution_changed() -> void:
	if is_inside_tree():
		_init_data(_resolution, true)
		_queue_rebuild()

func _on_material_changed() -> void:
	if is_inside_tree() and _material:
		_apply_material_uniforms()

func _queue_rebuild() -> void:
	if _rebuild_queued or _mesh == null:
		return
	_rebuild_queued = true
	call_deferred("_do_rebuild")

func _do_rebuild() -> void:
	_rebuild_queued = false
	_build_mesh()

func _apply_material_uniforms() -> void:
	if _material == null:
		return
	var letters := ["a", "b", "c", "d"]
	for i in 4:
		var u := letters[i]
		_material.set_shader_parameter("albedo_" + u, _albedo[i])
		_material.set_shader_parameter("roughness_" + u, _roughness[i])
		_material.set_shader_parameter("metallic_" + u, _metallic[i])
	_material.set_shader_parameter("uv_scale", _uv_scale)

func _build_mesh() -> void:
	if _mesh == null or _w == 0:
		return
	if _material == null:
		_material = ShaderMaterial.new()
		_material.shader = _SHADER
	_apply_material_uniforms()
	var st := SurfaceTool.new()
	st.begin(Mesh.PRIMITIVE_TRIANGLES)
	st.set_material(_material)
	var inv_w := 1.0 / float(_w - 1)
	var cellx := _size.x / float(_w - 1)
	var cellz := _size.y / float(_w - 1)
	for y in _w:
		for x in _w:
			var i := y * _w + x
			var wx := (float(x) * inv_w - 0.5) * _size.x
			var wz := (float(y) * inv_w - 0.5) * _size.y
			var wy := _heights[i] * _height_scale
			var n := Vector3(
				(_h_at(x - 1, y) - _h_at(x + 1, y)) * _height_scale / (2.0 * cellx),
				1.0,
				(_h_at(x, y - 1) - _h_at(x, y + 1)) * _height_scale / (2.0 * cellz)
			).normalized()
			var wi := i * 4
			st.set_normal(n)
			st.set_color(Color(_weights[wi], _weights[wi + 1], _weights[wi + 2], _weights[wi + 3]))
			st.set_uv(Vector2(float(x) * inv_w, float(y) * inv_w))
			st.add_vertex(Vector3(wx, wy, wz))
	for y in _w - 1:
		for x in _w - 1:
			var a := y * _w + x
			var b := a + 1
			var c := a + _w
			var d := c + 1
			st.add_index(a)
			st.add_index(c)
			st.add_index(b)
			st.add_index(b)
			st.add_index(c)
			st.add_index(d)
	_mesh.mesh = st.commit()
