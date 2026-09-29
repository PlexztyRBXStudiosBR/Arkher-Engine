/**************************************************************************/
/*  arkher_quality_preset.cpp                                             */
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

#include "arkher_quality_preset.h"

#include "core/object/class_db.h"

void ArkherQualityPreset::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_target_fps", "fps"), &ArkherQualityPreset::set_target_fps);
	ClassDB::bind_method(D_METHOD("get_target_fps"), &ArkherQualityPreset::get_target_fps);

	ClassDB::bind_method(D_METHOD("set_min_scale", "scale"), &ArkherQualityPreset::set_min_scale);
	ClassDB::bind_method(D_METHOD("get_min_scale"), &ArkherQualityPreset::get_min_scale);

	ClassDB::bind_method(D_METHOD("set_max_scale", "scale"), &ArkherQualityPreset::set_max_scale);
	ClassDB::bind_method(D_METHOD("get_max_scale"), &ArkherQualityPreset::get_max_scale);

	ClassDB::bind_method(D_METHOD("set_upscaler", "upscaler"), &ArkherQualityPreset::set_upscaler);
	ClassDB::bind_method(D_METHOD("get_upscaler"), &ArkherQualityPreset::get_upscaler);

	ClassDB::bind_method(D_METHOD("set_fsr_sharpness", "sharpness"), &ArkherQualityPreset::set_fsr_sharpness);
	ClassDB::bind_method(D_METHOD("get_fsr_sharpness"), &ArkherQualityPreset::get_fsr_sharpness);

	ClassDB::bind_method(D_METHOD("set_shadows_per_tier", "values"), &ArkherQualityPreset::set_shadows_per_tier);
	ClassDB::bind_method(D_METHOD("get_shadows_per_tier"), &ArkherQualityPreset::get_shadows_per_tier);

	ClassDB::bind_method(D_METHOD("set_shadow_distance_per_tier", "values"), &ArkherQualityPreset::set_shadow_distance_per_tier);
	ClassDB::bind_method(D_METHOD("get_shadow_distance_per_tier"), &ArkherQualityPreset::get_shadow_distance_per_tier);

	ClassDB::bind_method(D_METHOD("set_reflections_per_tier", "values"), &ArkherQualityPreset::set_reflections_per_tier);
	ClassDB::bind_method(D_METHOD("get_reflections_per_tier"), &ArkherQualityPreset::get_reflections_per_tier);

	ClassDB::bind_method(D_METHOD("set_sdfgi_per_tier", "values"), &ArkherQualityPreset::set_sdfgi_per_tier);
	ClassDB::bind_method(D_METHOD("get_sdfgi_per_tier"), &ArkherQualityPreset::get_sdfgi_per_tier);

	ClassDB::bind_method(D_METHOD("set_ssao_per_tier", "values"), &ArkherQualityPreset::set_ssao_per_tier);
	ClassDB::bind_method(D_METHOD("get_ssao_per_tier"), &ArkherQualityPreset::get_ssao_per_tier);

	ClassDB::bind_method(D_METHOD("set_ssil_per_tier", "values"), &ArkherQualityPreset::set_ssil_per_tier);
	ClassDB::bind_method(D_METHOD("get_ssil_per_tier"), &ArkherQualityPreset::get_ssil_per_tier);

	ClassDB::bind_method(D_METHOD("set_glow_per_tier", "values"), &ArkherQualityPreset::set_glow_per_tier);
	ClassDB::bind_method(D_METHOD("get_glow_per_tier"), &ArkherQualityPreset::get_glow_per_tier);

	ClassDB::bind_method(D_METHOD("set_volumetric_per_tier", "values"), &ArkherQualityPreset::set_volumetric_per_tier);
	ClassDB::bind_method(D_METHOD("get_volumetric_per_tier"), &ArkherQualityPreset::get_volumetric_per_tier);

	ClassDB::bind_method(D_METHOD("set_ssr_per_tier", "values"), &ArkherQualityPreset::set_ssr_per_tier);
	ClassDB::bind_method(D_METHOD("get_ssr_per_tier"), &ArkherQualityPreset::get_ssr_per_tier);

	ClassDB::bind_method(D_METHOD("set_thermal_heat_rate", "rate"), &ArkherQualityPreset::set_thermal_heat_rate);
	ClassDB::bind_method(D_METHOD("get_thermal_heat_rate"), &ArkherQualityPreset::get_thermal_heat_rate);

	ClassDB::bind_method(D_METHOD("set_thermal_cool_rate", "rate"), &ArkherQualityPreset::set_thermal_cool_rate);
	ClassDB::bind_method(D_METHOD("get_thermal_cool_rate"), &ArkherQualityPreset::get_thermal_cool_rate);

	ClassDB::bind_method(D_METHOD("set_thermal_warning_threshold", "threshold"), &ArkherQualityPreset::set_thermal_warning_threshold);
	ClassDB::bind_method(D_METHOD("get_thermal_warning_threshold"), &ArkherQualityPreset::get_thermal_warning_threshold);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "target_fps", PROPERTY_HINT_RANGE, "1,240,0.1"), "set_target_fps", "get_target_fps");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_scale", PROPERTY_HINT_RANGE, "0.1,1,0.01"), "set_min_scale", "get_min_scale");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_scale", PROPERTY_HINT_RANGE, "0.1,2,0.01"), "set_max_scale", "get_max_scale");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "upscaler", PROPERTY_HINT_ENUM, "Auto,Bilinear,FSR,FSR2"), "set_upscaler", "get_upscaler");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fsr_sharpness", PROPERTY_HINT_RANGE, "0,1,0.05"), "set_fsr_sharpness", "get_fsr_sharpness");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "shadows_per_tier"), "set_shadows_per_tier", "get_shadows_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "shadow_distance_per_tier"), "set_shadow_distance_per_tier", "get_shadow_distance_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "reflections_per_tier"), "set_reflections_per_tier", "get_reflections_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "sdfgi_per_tier"), "set_sdfgi_per_tier", "get_sdfgi_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "ssao_per_tier"), "set_ssao_per_tier", "get_ssao_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "ssil_per_tier"), "set_ssil_per_tier", "get_ssil_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "glow_per_tier"), "set_glow_per_tier", "get_glow_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "volumetric_per_tier"), "set_volumetric_per_tier", "get_volumetric_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "ssr_per_tier"), "set_ssr_per_tier", "get_ssr_per_tier");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "thermal_heat_rate", PROPERTY_HINT_RANGE, "0.001,0.5,0.001"), "set_thermal_heat_rate", "get_thermal_heat_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "thermal_cool_rate", PROPERTY_HINT_RANGE, "0.001,0.5,0.001"), "set_thermal_cool_rate", "get_thermal_cool_rate");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "thermal_warning_threshold", PROPERTY_HINT_RANGE, "0.1,0.95,0.01"), "set_thermal_warning_threshold", "get_thermal_warning_threshold");
}

void ArkherQualityPreset::set_target_fps(double p_target_fps) {
	target_fps = MAX(p_target_fps, 1.0);
}

double ArkherQualityPreset::get_target_fps() const {
	return target_fps;
}

void ArkherQualityPreset::set_min_scale(double p_min_scale) {
	min_scale = CLAMP(p_min_scale, 0.1, 1.0);
	min_scale = MIN(min_scale, max_scale);
}

double ArkherQualityPreset::get_min_scale() const {
	return min_scale;
}

void ArkherQualityPreset::set_max_scale(double p_max_scale) {
	max_scale = CLAMP(p_max_scale, 0.1, 2.0);
	max_scale = MAX(max_scale, min_scale);
}

double ArkherQualityPreset::get_max_scale() const {
	return max_scale;
}

void ArkherQualityPreset::set_upscaler(ArkherQualityDirector::Upscaler p_upscaler) {
	upscaler = p_upscaler;
}

ArkherQualityDirector::Upscaler ArkherQualityPreset::get_upscaler() const {
	return upscaler;
}

void ArkherQualityPreset::set_fsr_sharpness(double p_sharpness) {
	fsr_sharpness = CLAMP(p_sharpness, 0.0, 1.0);
}

double ArkherQualityPreset::get_fsr_sharpness() const {
	return fsr_sharpness;
}

void ArkherQualityPreset::set_shadows_per_tier(const PackedInt32Array &p_values) {
	shadows_per_tier = p_values;
}

PackedInt32Array ArkherQualityPreset::get_shadows_per_tier() const {
	return shadows_per_tier;
}

void ArkherQualityPreset::set_shadow_distance_per_tier(const PackedFloat32Array &p_values) {
	shadow_distance_per_tier = p_values;
}

PackedFloat32Array ArkherQualityPreset::get_shadow_distance_per_tier() const {
	return shadow_distance_per_tier;
}

void ArkherQualityPreset::set_reflections_per_tier(const PackedInt32Array &p_values) {
	reflections_per_tier = p_values;
}

PackedInt32Array ArkherQualityPreset::get_reflections_per_tier() const {
	return reflections_per_tier;
}

void ArkherQualityPreset::set_sdfgi_per_tier(const PackedInt32Array &p_values) {
	sdfgi_per_tier = p_values;
}

PackedInt32Array ArkherQualityPreset::get_sdfgi_per_tier() const {
	return sdfgi_per_tier;
}

void ArkherQualityPreset::set_ssao_per_tier(const PackedInt32Array &p_values) {
	ssao_per_tier = p_values;
}

PackedInt32Array ArkherQualityPreset::get_ssao_per_tier() const {
	return ssao_per_tier;
}

void ArkherQualityPreset::set_ssil_per_tier(const PackedInt32Array &p_values) {
	ssil_per_tier = p_values;
}

PackedInt32Array ArkherQualityPreset::get_ssil_per_tier() const {
	return ssil_per_tier;
}

void ArkherQualityPreset::set_glow_per_tier(const PackedInt32Array &p_values) {
	glow_per_tier = p_values;
}

PackedInt32Array ArkherQualityPreset::get_glow_per_tier() const {
	return glow_per_tier;
}

void ArkherQualityPreset::set_volumetric_per_tier(const PackedInt32Array &p_values) {
	volumetric_per_tier = p_values;
}

PackedInt32Array ArkherQualityPreset::get_volumetric_per_tier() const {
	return volumetric_per_tier;
}

void ArkherQualityPreset::set_ssr_per_tier(const PackedInt32Array &p_values) {
	ssr_per_tier = p_values;
}

PackedInt32Array ArkherQualityPreset::get_ssr_per_tier() const {
	return ssr_per_tier;
}

void ArkherQualityPreset::set_thermal_heat_rate(double p_rate) {
	thermal_heat_rate = CLAMP(p_rate, 0.001, 0.5);
}

double ArkherQualityPreset::get_thermal_heat_rate() const {
	return thermal_heat_rate;
}

void ArkherQualityPreset::set_thermal_cool_rate(double p_rate) {
	thermal_cool_rate = CLAMP(p_rate, 0.001, 0.5);
}

double ArkherQualityPreset::get_thermal_cool_rate() const {
	return thermal_cool_rate;
}

void ArkherQualityPreset::set_thermal_warning_threshold(double p_threshold) {
	thermal_warning_threshold = CLAMP(p_threshold, 0.1, 0.95);
}

double ArkherQualityPreset::get_thermal_warning_threshold() const {
	return thermal_warning_threshold;
}
