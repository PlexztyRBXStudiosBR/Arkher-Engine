@tool
class_name ArkherQualityProfile
extends Resource
## Perfil de qualidade para o ArkherQualityDirector (versão GDScript).
##
## Crie um perfil por dispositivo (ex.: `profile_a70.tres`, `profile_top.tres`)
## e carregue com `director.apply_preset(load("res://presets/profile_a70.tres"))`.
## No build Arkher (C++) a classe equivalente nativa é `ArkherQualityPreset`
## (com as taxas de teto térmico incluídas).
##
## Os nomes das propriedades batem com os do `ArkherQualityDirector`
## (GDScript) — o apply_preset cop só o que existir nos dois.

## FPS alvo.
@export_range(1.0, 240.0, 0.1) var target_fps := 60.0
## Escala mínima de render.
@export_range(0.1, 1.0, 0.01) var min_scale := 0.4
## Escala máxima de render.
@export_range(0.1, 1.0, 0.01) var max_scale := 1.0
## Sharpness do FSR.
@export_range(0.0, 1.0, 0.05) var fsr_sharpness := 0.2

## Sombras ligadas por tier [low, medium, high].
@export var shadows_enabled_per_tier: PackedInt32Array = PackedInt32Array([0, 1, 1])
## Distância máxima de sombra por tier.
@export var shadow_max_distance_per_tier: PackedFloat32Array = PackedFloat32Array([30.0, 60.0, 100.0])
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
## Reflexos por tier.
@export var reflections_per_tier: PackedInt32Array = PackedInt32Array([0, 0, 1])
## MSAA por tier (0 off, 1 2x, 2 4x, 3 8x).
@export var msaa_per_tier: PackedInt32Array = PackedInt32Array([0, 1, 2])
