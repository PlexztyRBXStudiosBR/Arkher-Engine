@tool
extends EditorPlugin
## Plugin do editor Arkher Quality — registra o tipo ArkherQualityDirector.

const DirectorScript := preload("res://addons/arkher_quality/arkher_quality_director.gd")
const ICON := preload("res://addons/arkher_quality/icons/arkher_quality.svg")

func _enter_tree() -> void:
	add_custom_type("ArkherQualityDirector", "Node", DirectorScript, ICON)

func _exit_tree() -> void:
	remove_custom_type("ArkherQualityDirector")
