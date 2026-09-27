/**************************************************************************/
/*  arkher_quality_director.cpp                                           */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "arkher_quality_director.h"

#include "core/config/engine.h"
#include "core/math/math_funcs.h"
#include "core/object/class_db.h"
#include "core/os/os.h"
#include "scene/3d/light_3d.h"
#include "scene/3d/world_environment.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"
#include "scene/main/window.h"
#include "scene/resources/environment.h"

void ArkherQualityDirector::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &ArkherQualityDirector::set_enabled);
	ClassDB::bind_method(D_METHOD("is_enabled"), &ArkherQualityDirector::is_enabled);

	ClassDB::bind_method(D_METHOD("set_target_fps", "fps"), &ArkherQualityDirector::set_target_fps);
	ClassDB::bind_method(D_METHOD("get_target_fps"), &ArkherQualityDirector::get_target_fps);

	ClassDB::bind_method(D_METHOD("set_min_scale", "scale"), &ArkherQualityDirector::set_min_scale);
	ClassDB::bind_method(D_METHOD("get_min_scale"), &ArkherQualityDirector::get_min_scale);

	ClassDB::bind_method(D_METHOD("set_max_scale", "scale"), &ArkherQualityDirector::set_max_scale);
	ClassDB::bind_method(D_METHOD("get_max_scale"), &ArkherQualityDirector::get_max_scale);

	ClassDB::bind_method(D_METHOD("set_down_after_seconds", "seconds"), &ArkherQualityDirector::set_down_after_seconds);
	ClassDB::bind_method(D_METHOD("get_down_after_seconds"), &ArkherQualityDirector::get_down_after_seconds);

	ClassDB::bind_method(D_METHOD("set_up_after_seconds", "seconds"), &ArkherQualityDirector::set_up_after_seconds);
	ClassDB::bind_method(D_METHOD("get_up_after_seconds"), &ArkherQualityDirector::get_up_after_seconds);

	ClassDB::bind_method(D_METHOD("set_step_down", "step"), &ArkherQualityDirector::set_step_down);
	ClassDB::bind_method(D_METHOD("get_step_down"), &ArkherQualityDirector::get_step_down);

	ClassDB::bind_method(D_METHOD("set_step_up", "step"), &ArkherQualityDirector::set_step_up);
	ClassDB::bind_method(D_METHOD("get_step_up"), &ArkherQualityDirector::get_step_up);

	ClassDB::bind_method(D_METHOD("set_upscaler", "upscaler"), &ArkherQualityDirector::set_upscaler);
	ClassDB::bind_method(D_METHOD("get_upscaler"), &ArkherQualityDirector::get_upscaler);

	ClassDB::bind_method(D_METHOD("set_fsr_sharpness", "sharpness"), &ArkherQualityDirector::set_fsr_sharpness);
	ClassDB::bind_method(D_METHOD("get_fsr_sharpness"), &ArkherQualityDirector::get_fsr_sharpness);

	ClassDB::bind_method(D_METHOD("set_auto_configure_scale_mode", "enabled"), &ArkherQualityDirector::set_auto_configure_scale_mode);
	ClassDB::bind_method(D_METHOD("get_auto_configure_scale_mode"), &ArkherQualityDirector::get_auto_configure_scale_mode);

	ClassDB::bind_method(D_METHOD("set_world_environment", "path"), &ArkherQualityDirector::set_world_environment);
	ClassDB::bind_method(D_METHOD("get_world_environment"), &ArkherQualityDirector::get_world_environment);

	ClassDB::bind_method(D_METHOD("set_shadows_per_tier", "values"), &ArkherQualityDirector::set_shadows_per_tier);
	ClassDB::bind_method(D_METHOD("get_shadows_per_tier"), &ArkherQualityDirector::get_shadows_per_tier);

	ClassDB::bind_method(D_METHOD("set_shadow_distance_per_tier", "values"), &ArkherQualityDirector::set_shadow_distance_per_tier);
	ClassDB::bind_method(D_METHOD("get_shadow_distance_per_tier"), &ArkherQualityDirector::get_shadow_distance_per_tier);

	ClassDB::bind_method(D_METHOD("set_sdfgi_per_tier", "values"), &ArkherQualityDirector::set_sdfgi_per_tier);
	ClassDB::bind_method(D_METHOD("get_sdfgi_per_tier"), &ArkherQualityDirector::get_sdfgi_per_tier);

	ClassDB::bind_method(D_METHOD("set_ssao_per_tier", "values"), &ArkherQualityDirector::set_ssao_per_tier);
	ClassDB::bind_method(D_METHOD("get_ssao_per_tier"), &ArkherQualityDirector::get_ssao_per_tier);

	ClassDB::bind_method(D_METHOD("set_ssil_per_tier", "values"), &ArkherQualityDirector::set_ssil_per_tier);
	ClassDB::bind_method(D_METHOD("get_ssil_per_tier"), &ArkherQualityDirector::get_ssil_per_tier);

	ClassDB::bind_method(D_METHOD("set_glow_per_tier", "values"), &ArkherQualityDirector::set_glow_per_tier);
	ClassDB::bind_method(D_METHOD("get_glow_per_tier"), &ArkherQualityDirector::get_glow_per_tier);

	ClassDB::bind_method(D_METHOD("set_volumetric_per_tier", "values"), &ArkherQualityDirector::set_volumetric_per_tier);
	ClassDB::bind_method(D_METHOD("get_volumetric_per_tier"), &ArkherQualityDirector::get_volumetric_per_tier);

	ClassDB::bind_method(D_METHOD("set_ssr_per_tier", "values"), &ArkherQualityDirector::set_ssr_per_tier);
	ClassDB::bind_method(D_METHOD("get_ssr_per_tier"), &ArkherQualityDirector::get_ssr_per_tier);

	ClassDB::bind_method(D_METHOD("set_msaa_per_tier", "values"), &ArkherQualityDirector::set_msaa_per_tier);
	ClassDB::bind_method(D_METHOD("get_msaa_per_tier"), &ArkherQualityDirector::get_msaa_per_tier);

	ClassDB::bind_method(D_METHOD("set_thermal_heat_rate", "rate"), &ArkherQualityDirector::set_thermal_heat_rate);
	ClassDB::bind_method(D_METHOD("get_thermal_heat_rate"), &ArkherQualityDirector::get_thermal_heat_rate);

	ClassDB::bind_method(D_METHOD("set_thermal_cool_rate", "rate"), &ArkherQualityDirector::set_thermal_cool_rate);
	ClassDB::bind_method(D_METHOD("get_thermal_cool_rate"), &ArkherQualityDirector::get_thermal_cool_rate);

	ClassDB::bind_method(D_METHOD("set_thermal_warning_threshold", "threshold"), &ArkherQualityDirector::set_thermal_warning_threshold);
	ClassDB::bind_method(D_METHOD("get_thermal_warning_threshold"), &ArkherQualityDirector::get_thermal_warning_threshold);

	ClassDB::bind_method(D_METHOD("get_thermal_state"), &ArkherQualityDirector::get_thermal_state);
	ClassDB::bind_method(D_METHOD("get_effective_max_scale"), &ArkherQualityDirector::get_effective_max_scale);
	ClassDB::bind_method(D_METHOD("get_current_scale"), &ArkherQualityDirector::get_current_scale);
	ClassDB::bind_method(D_METHOD("get_current_tier"), &ArkherQualityDirector::get_current_tier);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "target_fps", PROPERTY_HINT_RANGE, "1,240,0.1"), "set_target_fps", "get_target_fps");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_scale", PROPERTY_HINT_RANGE, "0.1,1,0.01"), "set_min_scale", "get_min_scale");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_scale", PROPERTY_HINT_RANGE, "0.1,2,0.01"), "set_max_scale", "get_max_scale");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "down_after_seconds", PROPERTY_HINT_RANGE, "0.05,10,0.05"), "set_down_after_seconds", "get_down_after_seconds");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "up_after_seconds", PROPERTY_HINT_RANGE, "0.05,30,0.05"), "set_up_after_seconds", "get_up_after_seconds");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "step_down", PROPERTY_HINT_RANGE, "0.02,0.5,0.01"), "set_step_down", "get_step_down");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "step_up", PROPERTY_HINT_RANGE, "0.01,0.25,0.01"), "set_step_up", "get_step_up");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "upscaler", PROPERTY_HINT_ENUM, "Auto,Bilinear,FSR,FSR2"), "set_upscaler", "get_upscaler");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fsr_sharpness", PROPERTY_HINT_RANGE, "0,1,0.05"), "set_fsr_sharpness", "get_fsr_sharpness");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_configure_scale_mode"), "set_auto_configure_scale_mode", "get_auto_configure_scale_mode");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "world_environment"), "set_world_environment", "get_world_environment");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "shadows_per_tier"), "set_shadows_per_tier", "get_shadows_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "shadow_distance_per_tier"), "set_shadow_distance_per_tier", "get_shadow_distance_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "sdfgi_per_tier"), "set_sdfgi_per_tier", "get_sdfgi_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "ssao_per_tier"), "set_ssao_per_tier", "get_ssao_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "ssil_per_tier"), "set_ssil_per_tier", "get_ssil_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "glow_per_tier"), "set_glow_per_tier", "get_glow_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "volumetric_per_tier"), "set_volumetric_per_tier", "get_volumetric_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "ssr_per_tier"), "set_ssr_per_tier", "get_ssr_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "msaa_per_tier"), "set_msaa_per_tier", "get_msaa_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "thermal_heat_rate", PROPERTY_HINT_RANGE, "0.001,0.5,0.001"), "set_thermal_heat_rate", "get_thermal_heat_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "thermal_cool_rate", PROPERTY_HINT_RANGE, "0.001,0.5,0.001"), "set_thermal_cool_rate", "get_thermal_cool_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "thermal_warning_threshold", PROPERTY_HINT_RANGE, "0.1,0.95,0.01"), "set_thermal_warning_threshold", "get_thermal_warning_threshold");

	ADD_SIGNAL(MethodInfo("scale_changed", PropertyInfo(Variant::FLOAT, "scale")));
	ADD_SIGNAL(MethodInfo("tier_changed", PropertyInfo(Variant::INT, "tier")));
	ADD_SIGNAL(MethodInfo("quality_changed", PropertyInfo(Variant::FLOAT, "scale"), PropertyInfo(Variant::INT, "tier")));
	ADD_SIGNAL(MethodInfo("thermal_warning", PropertyInfo(Variant::BOOL, "warning")));
}

Window *ArkherQualityDirector::_get_root_window() const {
	const SceneTree *tree = get_tree();
	if (tree == nullptr) {
		return nullptr;
	}
	return static_cast<Window *>(tree->get_root());
}

void ArkherQualityDirector::_configure_upscaler() {
	Window *root = _get_root_window();
	if (root == nullptr) {
		return;
	}

	if (auto_configure_scale_mode && root->get_content_scale_mode() == Window::CONTENT_SCALE_MODE_DISABLED) {
		root->set_content_scale_mode(Window::CONTENT_SCALE_MODE_VIEWPORT);
		root->set_content_scale_aspect(Window::CONTENT_SCALE_ASPECT_IGNORE);
	}

	Viewport::Scaling3DMode mode = Viewport::SCALING_3D_MODE_BILINEAR;
	if (upscaler == UPSCALER_FSR) {
		mode = Viewport::SCALING_3D_MODE_FSR;
	} else if (upscaler == UPSCALER_FSR2) {
		mode = Viewport::SCALING_3D_MODE_FSR2;
	} else if (upscaler == UPSCALER_AUTO) {
		// This engine ships FSR; the GDScript director picks FSR2 when the
		// game wants the temporal version, so AUTO = FSR here.
		mode = Viewport::SCALING_3D_MODE_FSR;
	}
	root->set_scaling_3d_mode(mode);
	root->set_fsr_sharpness((float)fsr_sharpness);
}

void ArkherQualityDirector::_apply_scale() {
	Window *root = _get_root_window();
	if (root == nullptr) {
		return;
	}
	root->set_content_scale_factor((float)current_scale);
	emit_signal("scale_changed", current_scale);
}

int ArkherQualityDirector::_tier_for_scale(double p_scale) const {
	if (p_scale >= 0.85) {
		return TIER_HIGH;
	}
	if (p_scale >= 0.65) {
		return TIER_MEDIUM;
	}
	return TIER_LOW;
}

int ArkherQualityDirector::_tier_value(const PackedInt32Array &p_values, int p_default) const {
	if (current_tier >= 0 && current_tier < p_values.size()) {
		return p_values[current_tier];
	}
	return p_default;
}

double ArkherQualityDirector::_tier_float(const PackedFloat32Array &p_values, double p_default) const {
	if (current_tier >= 0 && current_tier < p_values.size()) {
		return (double)p_values[current_tier];
	}
	return p_default;
}

double ArkherQualityDirector::_thermal_ceiling() const {
	if (thermal_state <= thermal_warning_threshold) {
		return 1.0;
	}
	const double span = MAX(1.0 - thermal_warning_threshold, 0.01);
	const double t = (thermal_state - thermal_warning_threshold) / span;
	// At thermal = 1 the effective max scale is halved.
	return 1.0 - 0.5 * t;
}

void ArkherQualityDirector::_apply_tier() {
	if (world_environment) {
		Ref<Environment> env = world_environment->get_environment();
		if (env.is_valid()) {
			env->set_sdfgi_enabled(_tier_value(sdfgi_per_tier, 0) == 1);
			env->set_ssao_enabled(_tier_value(ssao_per_tier, 0) == 1);
			env->set_ssil_enabled(_tier_value(ssil_per_tier, 0) == 1);
			env->set_glow_enabled(_tier_value(glow_per_tier, 0) == 1);
			env->set_volumetric_fog_enabled(_tier_value(volumetric_per_tier, 0) == 1);
			env->set_ssr_enabled(_tier_value(ssr_per_tier, 0) == 1);
			env->set_reflection_source(current_tier == TIER_LOW ? Environment::REFLECTION_SOURCE_DISABLED : Environment::REFLECTION_SOURCE_BG);
		}
	}

	const SceneTree *tree = get_tree();
	if (tree == nullptr) {
		return;
	}
	Window *root = static_cast<Window *>(tree->get_root());
	if (root == nullptr) {
		return;
	}

	const bool shadows_on = _tier_value(shadows_per_tier, 1) == 1;
	const float shadow_distance = (float)_tier_float(shadow_distance_per_tier, 100.0);
	const TypedArray<Node> lights = root->find_children("", "Light3D", true, false);
	for (int i = 0; i < lights.size(); i++) {
		Light3D *light = Object::cast_to<Light3D>(lights[i]);
		if (light) {
			light->set_shadow(shadows_on);
			if (shadows_on) {
				light->set_distance_fade_shadow(shadow_distance);
			}
		}
	}

	Viewport::MSAA msaa = Viewport::MSAA_DISABLED;
	switch (_tier_value(msaa_per_tier, 0)) {
		case 1:
			msaa = Viewport::MSAA_2X;
			break;
		case 2:
			msaa = Viewport::MSAA_4X;
			break;
		case 3:
			msaa = Viewport::MSAA_8X;
			break;
		default:
			msaa = Viewport::MSAA_DISABLED;
			break;
	}
	root->set_msaa_3d(msaa);
}

void ArkherQualityDirector::_check_tier(double p_effective_max) {
	const int tier = _tier_for_scale(current_scale);
	if (tier != current_tier) {
		current_tier = tier;
		_apply_tier();
		emit_signal("tier_changed", current_tier);
		emit_signal("quality_changed", current_scale, current_tier);
	}
}

void ArkherQualityDirector::_process(double p_delta) {
	if (!enabled || !is_inside_tree()) {
		return;
	}
	SceneTree *tree = get_tree();
	if (tree == nullptr) {
		return;
	}

	if (!configured) {
		configured = true;
		_configure_upscaler();
		_apply_tier();
	}

	const float fps = (float)Engine::get_singleton()->get_frames_per_second();
	if (fps <= 0.0f) {
		return;
	}
	const float dt = (float)p_delta;

	// --- Thermal model ("teto térmico"). ---
	// Pressure > 1 means the device is working harder than the target FPS.
	const float pressure = (float)CLAMP(target_fps / MAX((double)fps, 1.0), 0.0, 2.0);
	const float heat = (float)(pressure * current_scale * thermal_heat_rate) * dt;
	// Cools faster when the frame budget is met and the scale is low.
	const float cool_factor = (fps > (float)(target_fps * 1.05) ? 1.0f : 0.25f) * (float)(0.3 + 0.7 * (1.0 - current_scale));
	const float cool = (float)thermal_cool_rate * cool_factor * dt;
	thermal_state = CLAMP(thermal_state + heat - cool, 0.0, 1.0);

	const bool warning = thermal_state >= thermal_warning_threshold;
	if (warning != thermal_warning_active) {
		thermal_warning_active = warning;
		emit_signal("thermal_warning", warning);
	}

	const double effective_max = max_scale * _thermal_ceiling();

	// --- Dynamic resolution with hysteresis. ---
	if (fps < (float)(target_fps * 0.97)) {
		down_timer += dt;
		up_timer = 0.0;
		if (down_timer >= down_after_seconds && current_scale > min_scale) {
			down_timer = 0.0;
			current_scale = MAX(current_scale - step_down, MIN(min_scale, effective_max));
			_apply_scale();
			_check_tier(effective_max);
		}
	} else if (fps > (float)(target_fps * 1.03)) {
		up_timer += dt;
		down_timer = 0.0;
		if (up_timer >= up_after_seconds && current_scale < effective_max) {
			up_timer = 0.0;
			current_scale = MIN(current_scale + step_up, effective_max);
			_apply_scale();
			_check_tier(effective_max);
		}
	} else {
		down_timer = 0.0;
		up_timer = 0.0;
	}
}

// ------------------------------------------------------------------ setters

void ArkherQualityDirector::set_enabled(bool p_enabled) {
	enabled = p_enabled;
}

bool ArkherQualityDirector::is_enabled() const {
	return enabled;
}

void ArkherQualityDirector::set_target_fps(double p_target_fps) {
	target_fps = MAX(p_target_fps, 1.0);
}

double ArkherQualityDirector::get_target_fps() const {
	return target_fps;
}

void ArkherQualityDirector::set_min_scale(double p_min_scale) {
	min_scale = CLAMP(p_min_scale, 0.1, 1.0);
	min_scale = MIN(min_scale, max_scale);
}

double ArkherQualityDirector::get_min_scale() const {
	return min_scale;
}

void ArkherQualityDirector::set_max_scale(double p_max_scale) {
	max_scale = CLAMP(p_max_scale, 0.1, 2.0);
	max_scale = MAX(max_scale, min_scale);
}

double ArkherQualityDirector::get_max_scale() const {
	return max_scale;
}

void ArkherQualityDirector::set_down_after_seconds(double p_seconds) {
	down_after_seconds = MAX(p_seconds, 0.05);
}

double ArkherQualityDirector::get_down_after_seconds() const {
	return down_after_seconds;
}

void ArkherQualityDirector::set_up_after_seconds(double p_seconds) {
	up_after_seconds = MAX(p_seconds, 0.05);
}

double ArkherQualityDirector::get_up_after_seconds() const {
	return up_after_seconds;
}

void ArkherQualityDirector::set_step_down(double p_step) {
	step_down = CLAMP(p_step, 0.02, 0.5);
}

double ArkherQualityDirector::get_step_down() const {
	return step_down;
}

void ArkherQualityDirector::set_step_up(double p_step) {
	step_up = CLAMP(p_step, 0.01, 0.25);
}

double ArkherQualityDirector::get_step_up() const {
	return step_up;
}

void ArkherQualityDirector::set_upscaler(Upscaler p_upscaler) {
	upscaler = p_upscaler;
	if (is_inside_tree()) {
		_configure_upscaler();
	}
}

ArkherQualityDirector::Upscaler ArkherQualityDirector::get_upscaler() const {
	return upscaler;
}

void ArkherQualityDirector::set_fsr_sharpness(double p_sharpness) {
	fsr_sharpness = CLAMP(p_sharpness, 0.0, 1.0);
	if (is_inside_tree()) {
		Window *root = _get_root_window();
		if (root) {
			root->set_fsr_sharpness((float)fsr_sharpness);
		}
	}
}

double ArkherQualityDirector::get_fsr_sharpness() const {
	return fsr_sharpness;
}

void ArkherQualityDirector::set_auto_configure_scale_mode(bool p_enabled) {
	auto_configure_scale_mode = p_enabled;
	if (is_inside_tree()) {
		_configure_upscaler();
	}
}

bool ArkherQualityDirector::get_auto_configure_scale_mode() const {
	return auto_configure_scale_mode;
}

void ArkherQualityDirector::set_world_environment(const NodePath &p_path) {
	world_environment_path = p_path;
	world_environment = nullptr;
	if (is_inside_tree() && !p_path.is_empty()) {
		Node *n = get_node_or_null(p_path);
		world_environment = Object::cast_to<WorldEnvironment>(n);
	}
}

NodePath ArkherQualityDirector::get_world_environment() const {
	return world_environment_path;
}

void ArkherQualityDirector::set_shadows_per_tier(const PackedInt32Array &p_values) {
	shadows_per_tier = p_values;
}

PackedInt32Array ArkherQualityDirector::get_shadows_per_tier() const {
	return shadows_per_tier;
}

void ArkherQualityDirector::set_shadow_distance_per_tier(const PackedFloat32Array &p_values) {
	shadow_distance_per_tier = p_values;
}

PackedFloat32Array ArkherQualityDirector::get_shadow_distance_per_tier() const {
	return shadow_distance_per_tier;
}

void ArkherQualityDirector::set_sdfgi_per_tier(const PackedInt32Array &p_values) {
	sdfgi_per_tier = p_values;
}

PackedInt32Array ArkherQualityDirector::get_sdfgi_per_tier() const {
	return sdfgi_per_tier;
}

void ArkherQualityDirector::set_ssao_per_tier(const PackedInt32Array &p_values) {
	ssao_per_tier = p_values;
}

PackedInt32Array ArkherQualityDirector::get_ssao_per_tier() const {
	return ssao_per_tier;
}

void ArkherQualityDirector::set_ssil_per_tier(const PackedInt32Array &p_values) {
	ssil_per_tier = p_values;
}

PackedInt32Array ArkherQualityDirector::get_ssil_per_tier() const {
	return ssil_per_tier;
}

void ArkherQualityDirector::set_glow_per_tier(const PackedInt32Array &p_values) {
	glow_per_tier = p_values;
}

PackedInt32Array ArkherQualityDirector::get_glow_per_tier() const {
	return glow_per_tier;
}

void ArkherQualityDirector::set_volumetric_per_tier(const PackedInt32Array &p_values) {
	volumetric_per_tier = p_values;
}

PackedInt32Array ArkherQualityDirector::get_volumetric_per_tier() const {
	return volumetric_per_tier;
}

void ArkherQualityDirector::set_ssr_per_tier(const PackedInt32Array &p_values) {
	ssr_per_tier = p_values;
}

PackedInt32Array ArkherQualityDirector::get_ssr_per_tier() const {
	return ssr_per_tier;
}

void ArkherQualityDirector::set_msaa_per_tier(const PackedInt32Array &p_values) {
	msaa_per_tier = p_values;
}

PackedInt32Array ArkherQualityDirector::get_msaa_per_tier() const {
	return msaa_per_tier;
}

void ArkherQualityDirector::set_thermal_heat_rate(double p_rate) {
	thermal_heat_rate = CLAMP(p_rate, 0.001, 0.5);
}

double ArkherQualityDirector::get_thermal_heat_rate() const {
	return thermal_heat_rate;
}

void ArkherQualityDirector::set_thermal_cool_rate(double p_rate) {
	thermal_cool_rate = CLAMP(p_rate, 0.001, 0.5);
}

double ArkherQualityDirector::get_thermal_cool_rate() const {
	return thermal_cool_rate;
}

void ArkherQualityDirector::set_thermal_warning_threshold(double p_threshold) {
	thermal_warning_threshold = CLAMP(p_threshold, 0.1, 0.95);
}

double ArkherQualityDirector::get_thermal_warning_threshold() const {
	return thermal_warning_threshold;
}

double ArkherQualityDirector::get_thermal_state() const {
	return thermal_state;
}

double ArkherQualityDirector::get_effective_max_scale() const {
	return max_scale * _thermal_ceiling();
}

double ArkherQualityDirector::get_current_scale() const {
	return current_scale;
}

int ArkherQualityDirector::get_current_tier() const {
	return current_tier;
}
