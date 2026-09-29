/**************************************************************************/
/*  arkher_quality_preset.h                                               */
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

#include "arkher_quality_director.h"

#include "core/io/resource.h"

/**
 * @brief A saved quality profile for the ArkherQualityDirector.
 *
 * Create one preset per target device (e.g. "a70.tres", "midrange.tres",
 * "top.tres") and load it at runtime with
 * ArkherQualityDirector::apply_preset(). This is how a solo dev calibrates
 * the "photoreal on any phone" pillar without hardcoding per-device values.
 */
class ArkherQualityPreset : public Resource {
	GDCLASS(ArkherQualityPreset, Resource)

public:
	void set_target_fps(double p_target_fps);
	double get_target_fps() const;

	void set_min_scale(double p_min_scale);
	double get_min_scale() const;

	void set_max_scale(double p_max_scale);
	double get_max_scale() const;

	void set_upscaler(ArkherQualityDirector::Upscaler p_upscaler);
	ArkherQualityDirector::Upscaler get_upscaler() const;

	void set_fsr_sharpness(double p_sharpness);
	double get_fsr_sharpness() const;

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

	void set_thermal_heat_rate(double p_rate);
	double get_thermal_heat_rate() const;

	void set_thermal_cool_rate(double p_rate);
	double get_thermal_cool_rate() const;

	void set_thermal_warning_threshold(double p_threshold);
	double get_thermal_warning_threshold() const;

protected:
	static void _bind_methods();

private:
	double target_fps = 60.0;
	double min_scale = 0.4;
	double max_scale = 1.0;
	ArkherQualityDirector::Upscaler upscaler = ArkherQualityDirector::UPSCALER_AUTO;
	double fsr_sharpness = 0.2;

	PackedInt32Array shadows_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedFloat32Array shadow_distance_per_tier = PackedFloat32Array({ 30.0, 60.0, 100.0 });
	PackedInt32Array reflections_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedInt32Array sdfgi_per_tier = PackedInt32Array({ 0, 0, 1 });
	PackedInt32Array ssao_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedInt32Array ssil_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedInt32Array glow_per_tier = PackedInt32Array({ 0, 1, 1 });
	PackedInt32Array volumetric_per_tier = PackedInt32Array({ 0, 0, 1 });
	PackedInt32Array ssr_per_tier = PackedInt32Array({ 0, 0, 1 });

	double thermal_heat_rate = 0.04;
	double thermal_cool_rate = 0.02;
	double thermal_warning_threshold = 0.6;
};
