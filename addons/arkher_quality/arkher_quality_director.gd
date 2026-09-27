@tool
class_name ArkherQualityDirector
extends Node
## Arkher QualityDirector — pilar 1 da Arkher Engine ("fotorreal em qualquer
## celular").
##
## Mede o FPS real, ajusta a resolução de render dinamicamente (0.4..1.0) e
## troca TIER de qualidade (0 low / 1 medium / 2 high): sombras, SDFGI, SSAO,
## SSIL, glow, volumétrico, SSR e reflexos. Usa FSR/FSR2 quando o engine
## oferece (Godot 4.4+ e o fork Arkher 4.8-dev); em versões antigas cai
## para upscale bilinear — ainda assim com resolução dinâmica funcionando.
##
## Funciona em GDScript puro: roda no Godot estável E no fork Arkher, sem
## compilar nada. No M2 do ROADMAP a lógica ganha a versão C++
## (ArkherQualityDirector nativa, integrada ao renderer Arkher).
##
## Exemplo:
## [code]
## director.quality_changed.connect(func(scale, tier):
## 	print("Arkher: escala ", scale, " tier ", tier)
## end)
## [/code]

signal scale_changed(scale: float)
signal tier_changed(tier: int)
signal quality_changed(scale: float, tier: int)

const TIER_LOW := 0
const TIER_MEDIUM := 1
const TIER_HIGH := 2

@export_group("Auto-quality")
## Liga/desliga o diretor inteiro.
@export var enabled := true
## FPS alvo. O diretor baixa a escala até segurar o alvo.
@export var target_fps := 60.0
## Escala mínima de render (0.4 = 40% da resolução).
@export_range(0.25, 1.0, 0.05) var min_scale := 0.4
## Escala máxima de render.
@export_range(0.25, 1.0, 0.05) var max_scale := 1.0
## Segundos abaixo do alvo antes de DROPAR a escala (histerese).
@export var down_after_seconds := 0.5
## Segundos acima do alvo antes de GANHAR escala de volta.
@export var up_after_seconds := 2.0
## Quanto a escala cai de cada vez.
@export_range(0.02, 0.5, 0.01) var step_down := 0.1
## Quanto a escala sobe de cada vez.
@export_range(0.01, 0.25, 0.01) var step_up := 0.05

@export_group("Upscale")
## Usa FSR/FSR2 para o upscale (se o engine tiver).
@export var use_fsr := true
## Sharpness do FSR (0 = liso, 1 = mais nítido).
@export_range(0.0, 1.0, 0.05) var fsr_sharpness := 0.2
## Se o jogo não configurou content scale, o diretor configura sozinho.
@export var auto_configure_scale_mode := true

@export_group("Tiers")
## WorldEnvironment que o diretor liga/desliga efeitos. (Opcional.)
@export var world_environment: WorldEnvironment
## Distância máxima de sombra por tier [low, medium, high].
@export var shadow_max_distance_per_tier: PackedFloat32Array = PackedFloat32Array([30.0, 60.0, 100.0])
## Sombras ligadas por tier [low, medium, high].
@export var shadows_enabled_per_tier: PackedInt32Array = PackedInt32Array([0, 1, 1])
## SDFGI por tier.
@export var sdfgi_per_tier: PackedInt32Array = PackedInt32Array([0, 0, 1])
## SSAO por tier.
@export var ssao_per_tier: PackedInt32Array = PackedInt32Array([0, 1, 1])
## SSIL por tier.
@export var ssil_per_tier: PackedInt32Array = PackedInt32Array([0, 1, 1])
## Glow por tier.
@export var glow_per_tier: PackedInt32Array = PackedInt32Array([0, 1, 1])
## Fog volumétrico por tier.
@export var volumetric_per_tier: PackedInt32Array = PackedInt32Array([0, 0, 1])
## SSR por tier.
@export var ssr_per_tier: PackedInt32Array = PackedInt32Array([0, 0, 1])
## Reflexos (sdf) por tier.
@export var reflections_per_tier: PackedInt32Array = PackedInt32Array([0, 0, 1])
## MSAA por tier (0 off, 1 2x, 2 4x, 3 8x).
@export var msaa_per_tier: PackedInt32Array = PackedInt32Array([0, 1, 2])

@export_group("Debug")
## Mostra um overlay no canto com fps/escala/tier (ótimo para testar no
## celular: dá pra ver a auto-quality agindo).
@export var show_debug_overlay := true

var _scale := 1.0
var _tier: int = TIER_HIGH
var _down_timer := 0.0
var _up_timer := 0.0
var _overlay: Label
var _overlay_layer: CanvasLayer
var _overlay_tick := 0.0
var _configured := false

func _ready() -> void:
	if Engine.is_editor_hint() and not _is_playing():
		return
	_ensure_scale_mode()
	_ensure_fsr()
	_apply_scale(true)
	_apply_tier()
	if show_debug_overlay:
		_build_overlay()

func _is_playing() -> bool:
	var scene_tree := get_tree()
	if scene_tree == null:
		return false
	return scene_tree.current_scene != null and get_parent() == scene_tree.root

func _process(delta: float) -> void:
	if not enabled or not is_inside_tree():
		return
	if Engine.is_editor_hint() and not _is_playing():
		return
	var fps := Engine.get_frames_per_second()
	if fps <= 0.0:
		return
	# Histerese: só mexe na escala depois de ficar um tempo fora do alvo.
	if fps < target_fps * 0.97:
		_down_timer += delta
		_up_timer = 0.0
		if _down_timer >= down_after_seconds and _scale > min_scale:
			_down_timer = 0.0
			_scale = maxf(_scale - step_down, min_scale)
			_apply_scale()
			_check_tier()
	elif fps > target_fps * 1.03:
		_up_timer += delta
		_down_timer = 0.0
		if _up_timer >= up_after_seconds and _scale < max_scale:
			_up_timer = 0.0
			_scale = minf(_scale + step_up, max_scale)
			_apply_scale()
			_check_tier()
	else:
		_down_timer = 0.0
		_up_timer = 0.0
	if show_debug_overlay:
		_overlay_tick += delta
		if _overlay_tick >= 0.25:
			_overlay_tick = 0.0
			_update_overlay(fps)

# ------------------------------------------------------------------ escala
func _ensure_scale_mode() -> void:
	if not auto_configure_scale_mode:
		return
	var root := get_tree().root as Window
	if root == null:
		return
	if int(root.content_scale_mode) != 0:
		return  # o jogo já configurou — não pisoteamos.
	# Godot <= 4.4 tem CONTENT_SCALE_MODE_TEXTURE; o fork 4.8-dev trocou o
	# upscale para scaling_3d_mode (FSR) e usa CONTENT_SCALE_MODE_VIEWPORT.
	var consts: Dictionary = Window.get_enum_constants("ContentScaleMode")
	var mode := int(consts.get("CONTENT_SCALE_MODE_VIEWPORT", 1))
	if consts.has("CONTENT_SCALE_MODE_TEXTURE"):
		mode = int(consts["CONTENT_SCALE_MODE_TEXTURE"])
	root.content_scale_mode = mode
	var aspects: Dictionary = Window.get_enum_constants("ContentScaleAspect")
	if aspects.has("CONTENT_SCALE_ASPECT_IGNORE"):
		root.content_scale_aspect = int(aspects["CONTENT_SCALE_ASPECT_IGNORE"])
	_configured = true

func _ensure_fsr() -> void:
	if not use_fsr:
		return
	var vp := get_viewport()
	if vp == null:
		return
	var consts: Dictionary = Viewport.get_enum_constants("Scaling3DMode")
	if not consts.has("SCALING_3D_MODE_FSR"):
		return  # engine antigo: upscale bilinear mesmo.
	vp.scaling_3d_mode = int(consts["SCALING_3D_MODE_FSR"])
	if vp.has_method("set_fsr_sharpness"):
		vp.fsr_sharpness = fsr_sharpness

func _apply_scale(silent: bool = false) -> void:
	var root := get_tree().root as Window
	if root == null:
		return
	root.content_scale_factor = _scale
	if not silent:
		scale_changed.emit(_scale)

func _tier_for_scale(s: float) -> int:
	if s >= 0.85:
		return TIER_HIGH
	if s >= 0.65:
		return TIER_MEDIUM
	return TIER_LOW

func _check_tier() -> void:
	var t := _tier_for_scale(_scale)
	if t != _tier:
		_tier = t
		_apply_tier()
		tier_changed.emit(_tier)
		quality_changed.emit(_scale, _tier)

func get_current_scale() -> float:
	return _scale

func get_current_tier() -> int:
	return _tier

# ------------------------------------------------------------------ tiers
func _tier_value(arr: PackedInt32Array, default_value: int) -> int:
	if _tier >= 0 and _tier < arr.size():
		return int(arr[_tier])
	return default_value

func _tier_float(arr: PackedFloat32Array, default_value: float) -> float:
	if _tier >= 0 and _tier < arr.size():
		return float(arr[_tier])
	return default_value

func _apply_tier() -> void:
	var env: Environment = null
	if world_environment:
		env = world_environment.environment
	if env:
		env.sdfgi_enabled = _tier_value(sdfgi_per_tier, 0) == 1
		env.ssao_enabled = _tier_value(ssao_per_tier, 0) == 1
		env.ssil_enabled = _tier_value(ssil_per_tier, 0) == 1
		env.glow_enabled = _tier_value(glow_per_tier, 0) == 1
		env.volumetric_fog_enabled = _tier_value(volumetric_per_tier, 0) == 1
		env.ssr_enabled = _tier_value(ssr_per_tier, 0) == 1
		# Godot <= 4.4: reflections_enabled (bool). Fork Arkher 4.8-dev:
		# reflected_light_source (0 = Background, 1 = Disabled, 2 = Sky).
		var reflections_on := _tier_value(reflections_per_tier, 0) == 1
		if "reflections_enabled" in env:
			env.reflections_enabled = reflections_on
		elif "reflected_light_source" in env:
			env.reflected_light_source = 1 if not reflections_on else 0
	# Sombras: varre as luzes da cena.
	if get_tree() and get_tree().root:
		var shadows_on := _tier_value(shadows_enabled_per_tier, 1) == 1
		var max_dist := _tier_float(shadow_max_distance_per_tier, 100.0)
		for light in get_tree().root.find_children("", "Light3D", true, false):
			var l := light as Light3D
			if l:
				# Godot <= 4.4: shadow_enabled/shadow_max_distance.
				# Fork Arkher 4.8-dev: shadow/distance_fade_shadow.
				if "shadow_enabled" in l:
					l.shadow_enabled = shadows_on
					if shadows_on:
						l.shadow_max_distance = max_dist
				else:
					l.shadow = shadows_on
					if shadows_on:
						l.distance_fade_shadow = max_dist
	# MSAA no viewport raiz.
	var vp := get_viewport()
	if vp:
		var msaa_value := _tier_value(msaa_per_tier, 0)
		if vp.has_method("set_msaa"):
			vp.msaa = msaa_value
		elif vp.has_method("set_msaa_3d"):
			vp.msaa_3d = msaa_value

# ----------------------------------------------------------------- overlay
func _build_overlay() -> void:
	_overlay_layer = CanvasLayer.new()
	_overlay_layer.layer = 100
	add_child(_overlay_layer)
	_overlay = Label.new()
	_overlay.add_theme_font_size_override("font_size", 14)
	_overlay.position = Vector2(12, 12)
	_overlay_layer.add_child(_overlay)
	_update_overlay(Engine.get_frames_per_second())

func _update_overlay(fps: float) -> void:
	if _overlay == null:
		return
	var tier_name := "LOW"
	if _tier == TIER_MEDIUM:
		tier_name = "MED"
	elif _tier == TIER_HIGH:
		tier_name = "HIGH"
	_overlay.text = "ARKHER  fps %d  scale %.2f  tier %s  alvo %d" % [int(fps), _scale, tier_name, int(target_fps)]
