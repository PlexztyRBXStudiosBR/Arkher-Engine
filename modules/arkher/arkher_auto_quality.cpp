/**************************************************************************/
/*  arkher_auto_quality.cpp                                               */
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
/* permit persons to whom the Software is furnished to do so, subject to   */
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

#include "arkher_auto_quality.h"

#include "core/config/engine.h"
#include "core/object/class_db.h"
#include "core/os/os.h"

#include <math.h>

void ArkherAutoQuality::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &ArkherAutoQuality::set_enabled);
	ClassDB::bind_method(D_METHOD("is_enabled"), &ArkherAutoQuality::is_enabled);

	ClassDB::bind_method(D_METHOD("set_target_fps", "fps"), &ArkherAutoQuality::set_target_fps);
	ClassDB::bind_method(D_METHOD("get_target_fps"), &ArkherAutoQuality::get_target_fps);

	ClassDB::bind_method(D_METHOD("set_min_scale", "scale"), &ArkherAutoQuality::set_min_scale);
	ClassDB::bind_method(D_METHOD("get_min_scale"), &ArkherAutoQuality::get_min_scale);

	ClassDB::bind_method(D_METHOD("set_max_scale", "scale"), &ArkherAutoQuality::set_max_scale);
	ClassDB::bind_method(D_METHOD("get_max_scale"), &ArkherAutoQuality::get_max_scale);

	ClassDB::bind_method(D_METHOD("set_smoothing", "smoothing"), &ArkherAutoQuality::set_smoothing);
	ClassDB::bind_method(D_METHOD("get_smoothing"), &ArkherAutoQuality::get_smoothing);

	ClassDB::bind_method(D_METHOD("set_consider_low_processor_usage_mode", "consider"), &ArkherAutoQuality::set_consider_low_processor_usage_mode);
	ClassDB::bind_method(D_METHOD("get_consider_low_processor_usage_mode"), &ArkherAutoQuality::get_consider_low_processor_usage_mode);

	ClassDB::bind_method(D_METHOD("get_current_scale"), &ArkherAutoQuality::get_current_scale);
	ClassDB::bind_method(D_METHOD("get_measured_fps"), &ArkherAutoQuality::get_measured_fps);
	ClassDB::bind_method(D_METHOD("get_tier"), &ArkherAutoQuality::get_tier);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enabled"), "set_enabled", "is_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "target_fps", PROPERTY_HINT_RANGE, "1,240,0.1"), "set_target_fps", "get_target_fps");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_scale", PROPERTY_HINT_RANGE, "0.1,1,0.01"), "set_min_scale", "get_min_scale");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_scale", PROPERTY_HINT_RANGE, "0.1,2,0.01"), "set_max_scale", "get_max_scale");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "smoothing", PROPERTY_HINT_RANGE, "0.001,1,0.001"), "set_smoothing", "get_smoothing");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "consider_low_processor_usage_mode"), "set_consider_low_processor_usage_mode", "get_consider_low_processor_usage_mode");

	ADD_SIGNAL(MethodInfo("quality_changed",
			PropertyInfo(Variant::FLOAT, "scale"),
			PropertyInfo(Variant::INT, "tier")));
}

void ArkherAutoQuality::_process(double p_delta) {
	if (!enabled) {
		return;
	}

	const float fps = static_cast<float>(Engine::get_singleton()->get_frames_per_second());
	if (fps <= 0.0f) {
		return;
	}

	const float dt = static_cast<float>(p_delta);
	const float alpha = 1.0f - powf(1.0f - CLAMP(smoothing, 0.001f, 1.0f), dt * 60.0f);
	ema_fps = ema_fps > 0.0f ? lerpf(ema_fps, fps, alpha) : fps;

	float scale = max_scale;
	if (target_fps > 0.0f) {
		scale = CLAMP(ema_fps / target_fps, min_scale, max_scale);
	}

	if (consider_low_processor_usage_mode && OS::get_singleton()->is_in_low_processor_usage_mode()) {
		// The game (or a previous ArkherQualityDirector pass) asked for
		// power saving: go as low as we are allowed to.
		scale = MIN(scale, min_scale + 0.05f);
	}

	scale = CLAMP(scale, min_scale, max_scale);

	if (absf(scale - current_scale) > 0.001f) {
		current_scale = lerpf(current_scale, scale, MINF(alpha * 0.5f + 0.02f, 1.0f));
		_quality_changed();
	}
}

void ArkherAutoQuality::_quality_changed() {
	emit_signal("quality_changed", current_scale, get_tier());
}

bool ArkherAutoQuality::is_enabled() const {
	return enabled;
}

void ArkherAutoQuality::set_enabled(bool p_enabled) {
	enabled = p_enabled;
}

float ArkherAutoQuality::get_target_fps() const {
	return target_fps;
}

void ArkherAutoQuality::set_target_fps(float p_target_fps) {
	target_fps = MAXF(p_target_fps, 1.0f);
}

float ArkherAutoQuality::get_min_scale() const {
	return min_scale;
}

void ArkherAutoQuality::set_min_scale(float p_min_scale) {
	min_scale = CLAMP(p_min_scale, 0.1f, 1.0f);
	min_scale = MINF(min_scale, max_scale);
}

float ArkherAutoQuality::get_max_scale() const {
	return max_scale;
}

void ArkherAutoQuality::set_max_scale(float p_max_scale) {
	max_scale = CLAMP(p_max_scale, 0.1f, 2.0f);
	max_scale = MAXF(max_scale, min_scale);
}

float ArkherAutoQuality::get_smoothing() const {
	return smoothing;
}

void ArkherAutoQuality::set_smoothing(float p_smoothing) {
	smoothing = CLAMP(p_smoothing, 0.001f, 1.0f);
}

bool ArkherAutoQuality::get_consider_low_processor_usage_mode() const {
	return consider_low_processor_usage_mode;
}

void ArkherAutoQuality::set_consider_low_processor_usage_mode(bool p_consider) {
	consider_low_processor_usage_mode = p_consider;
}

float ArkherAutoQuality::get_current_scale() const {
	return current_scale;
}

float ArkherAutoQuality::get_measured_fps() const {
	return ema_fps;
}

int ArkherAutoQuality::get_tier() const {
	if (current_scale >= 0.85f) {
		return QUALITY_TIER_HIGH;
	}
	if (current_scale >= 0.65f) {
		return QUALITY_TIER_MEDIUM;
	}
	return QUALITY_TIER_LOW;
}
