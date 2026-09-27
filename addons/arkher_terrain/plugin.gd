@tool
extends EditorPlugin
## Plugin do editor Arkher Terrain (v0.1).
##
## Registra o tipo "ArkherTerrain", abre o painel no dock esquerdo e passa
## o terreno selecionado para o painel.

const PanelScript := preload("res://addons/arkher_terrain/panel.gd")
const TerrainScript := preload("res://addons/arkher_terrain/arkher_terrain.gd")
const ICON := preload("res://addons/arkher_terrain/icons/arkher_terrain.svg")

var _panel: Control = null

func _enter_tree() -> void:
	add_custom_type("ArkherTerrain", "Node3D", TerrainScript, ICON)
	_panel = PanelScript.new()
	_panel.editor_interface = get_editor_interface()
	add_control_to_dock(DOCK_SLOT_LEFT_UL, _panel)
	get_editor_interface().get_selection().selection_changed.connect(_on_selection_changed)

func _exit_tree() -> void:
	get_editor_interface().get_selection().selection_changed.disconnect(_on_selection_changed)
	remove_control_from_docks(_panel)
	_panel.queue_free()
	_panel = null
	remove_custom_type("ArkherTerrain")

func _on_selection_changed() -> void:
	if _panel == null:
		return
	_panel.terrain_changed(_find_selected_terrain())

func _find_selected_terrain() -> Node:
	for node in get_editor_interface().get_selection().get_selected_nodes():
		if node != null and node is ArkherTerrain:
			return node
	return null
