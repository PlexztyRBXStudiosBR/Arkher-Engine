/**************************************************************************/
/*  arkher_quality_director.h                                             */
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

#pragma once

#include "scene/main/node.h"

class Environment;
class WorldEnvironment;
class Window;
class ArkherQualityPreset;

/**
 * @brief Arkher pillar 1 ("photoreal on any phone"), native version.
 *
 * Unlike ArkherAutoQuality (advisory), this director APPLIES the quality
 * changes: it drives the window's content scale (dynamic resolution with
 * hysteresis), selects the upscaler (FSR/FSR2 when available) and applies
 * per-tier rendering settings (shadows, SDFGI, SSAO, SSIL, glow, volumetric
 * fog, SSR, reflections, MSAA) to a WorldEnvironment and the scene lights.
 *
 * A built-in thermal model ("teto térmico") accumulates heat proportional to
 * the sustained rendering load and lowers the effective max scale when the
 * device runs hot, keeping phones like the Itel A70 (the Arkher mobile floor
 * benchmark) inside a sustainable thermal budget.
 */
class ArkherQualityDirector : public Node {
	GDCLASS(ArkherQualityDirector, Node)

public:
	enum QualityTier {
		TIER_LOW = 0,
		TIER_MEDIUM = 1,
		TIER_HIGH = 2,
	};

	enum Upscaler {
		UPSCALER_AUTO,
		UPSCALER_BILINEAR,
		UPSCALER_FSR,
		UPSCALER_FSR2,
	};

	void set_enabled(bool p_enabled);
	bool is_enabled() const;

	void set_target_fps(double p_target_fps);
	double get_target_fps() const;

	void set_min_scale(double p_min_scale);
	double get_min_scale() const;

	void set_max_scale(double p_max_scale);
	double get_max_scale() const;

	void set_down_after_seconds(double p_seconds);
	double get_down_after_seconds() const;

	void set_up_after_seconds(double p_seconds);
	double get_up_after_seconds() const;

	void set_step_down(double p_step);
	double get_step_down() const;

	void set_step_up(double p_step);
	double get_step_up() const;

	void set_upscaler(Upscaler p_upscaler);
	Upscaler get_upscaler() const;

	void set_fsr_sharpness(double p_sharpness);
	double get_fsr_sharpness() const;

	void set_auto_configure_scale_mode(bool p_enabled);
	bool get_auto_configure_scale_mode() const;

	void set_world_environment(const NodePath &p_path);
	NodePath get_world_environment() const;

	void set_shadows_per_tier(const PackedInt32Array &p_values);
	PackedInt32Array get_shadows_per_tier() const;

	void set_shadow_distance_per_tier(const PackedFloat32Array &p_values);
	PackedFloat32Array get_shadow_distance_per_tier() const;

	void set_reflections_per_tier(const PackedInt32Array &p_values);
	PackedInt32Array get_reflections_per_tier() const;

	void set_sdfgi_per_tier(const PackedInt32Array &p_values);
	PackedInt32Array get_sdfgi_per_tier() const;

	void set_ssao_per_tier(const PackedInt32Array &p_values);
	PackedInt32Array get_ssao_per_tier() const;

	void set_ssil_per_tier(const PackedInt32Array &p_values);
	PackedInt32Array get_ssil_per_tier() const;

	void set_glow_per_tier(const PackedInt32Array &p_values);
	PackedInt32Array get_glow_per_tier() const;

	void set_volumetric_per_tier(const PackedInt32Array &p_values);
	PackedInt32Array get_volumetric_per_tier() const;

	void set_ssr_per_tier(const PackedInt32Array &p_values);
	PackedInt32Array get_ssr_per_tier() const;

	void set_msaa_per_tier(const PackedInt32Array &p_values);
	PackedInt32Array get_msaa_per_tier() const;

	/// How fast the thermal state rises at full load (per second, 0..1 scale).
	void set_thermal_heat_rate(double p_rate);
	double get_thermal_heat_rate() const;

	/// How fast the thermal state falls (per second, 0..1 scale).
	void set_thermal_cool_rate(double p_rate);
	double get_thermal_cool_rate() const;

	/// Thermal state at which the ceiling starts lowering quality (0..1).
	void set_thermal_warning_threshold(double p_threshold);
	double get_thermal_warning_threshold() const;

	/// Current thermal state, 0 (cold) to 1 (overheated).
	double get_thermal_state() const;

	/// max_scale reduced by the thermal ceiling (what the director will not exceed).
	double get_effective_max_scale() const;

	/// Current dynamic-resolution scale (min_scale..max_scale).
	double get_current_scale() const;

	/// Current applied tier (TIER_LOW / TIER_MEDIUM / TIER_HIGH).
	int get_current_tier() const;

	/// Loads a saved quality profile (per-device calibration) and applies it.
	void apply_preset(const Ref<ArkherQualityPreset> &p_preset);

	/// Profile loaded automatically on startup (e.g. the floor-device profile).
	void set_startup_preset(const Ref<ArkherQualityPreset> &p_preset);
	Ref<ArkherQualityPreset> get_startup_preset() const;

protected:
	void _process(double p_delta);

	void _configure_upscaler();
	void _apply_scale();
	void _check_tier(double p_effective_max);
	void _apply_tier();

	static void _bind_methods();

private:
	int _tier_value(const PackedInt32Array &p_values, int p_default) const;
	double _tier_float(const PackedFloat32Array &p_values, double p_default) const;
	int _tier_for_scale(double p_scale) const;
	double _thermal_ceiling() const;
	Window *_get_root_window() const;

	bool enabled = true;
	double target_fps = 60.0;
	double min_scale = 0.4;
	double max_scale = 1.0;
	double down_after_seconds = 0.5;
	double up_after_seconds = 2.0;
	double step_down = 0.1;
	double step_up = 0.05;
	Upscaler upscaler = UPSCALER_AUTO;
	double fsr_sharpness = 0.2;
	bool auto_configure_scale_mode = true;

	NodePath world_environment_path;
	WorldEnvironment *world_environment = nullptr;

	PackedInt32Array shadows_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedFloat32Array shadow_distance_per_tier = PackedFloat32Array({ 30.0, 60.0, 100.0 });
	PackedInt32Array reflections_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedInt32Array sdfgi_per_tier = PackedInt32Array({ 0, 0, 1 });
	PackedInt32Array ssao_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedInt32Array ssil_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedInt32Array glow_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedInt32Array volumetric_per_tier = PackedInt32Array({ 0, 0, 1 });
	PackedInt32Array ssr_per_tier = PackedInt32Array({ 0, 0, 1 });
	PackedInt32Array msaa_per_tier = PackedInt32Array({ 0, 1, 2 });

	Ref<ArkherQualityPreset> startup_preset;

	double thermal_heat_rate = 0.04;
	double thermal_cool_rate = 0.02;
	double thermal_warning_threshold = 0.6;

	double current_scale = 1.0;
	int current_tier = TIER_HIGH;
	double down_timer = 0.0;
	double up_timer = 0.0;
	double thermal_state = 0.0;
	bool thermal_warning_active = false;
	bool configured = false;
};

VARIANT_ENUM_CAST(ArkherQualityDirector::Upscaler);
VARIANT_ENUM_CAST(ArkherQualityDirector::QualityTier);
