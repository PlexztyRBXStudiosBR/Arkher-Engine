/**************************************************************************/
/*  arkher_auto_quality.h                                                 */
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

/**
 * @brief Arkher pillar 1 ("photoreal on any phone"): measures device
 * performance and recommends a rendering quality scale (0..1) plus a quality
 * tier (0/1/2) through the `quality_changed` signal.
 *
 * The node is *advisory*: it never changes rendering on its own. Games (or
 * the future ArkherQualityDirector) consume the signal to scale resolution,
 * shadows, GI and effect tiers, keeping the target FPS inside the thermal
 * budget of the device.
 */
class ArkherAutoQuality : public Node {
	GDCLASS(ArkherAutoQuality, Node)

public:
	enum QualityTier {
		QUALITY_TIER_LOW,
		QUALITY_TIER_MEDIUM,
		QUALITY_TIER_HIGH,
	};

	bool is_enabled() const;
	void set_enabled(bool p_enabled);

	float get_target_fps() const;
	void set_target_fps(float p_target_fps);

	float get_min_scale() const;
	void set_min_scale(float p_min_scale);

	float get_max_scale() const;
	void set_max_scale(float p_max_scale);

	float get_smoothing() const;
	void set_smoothing(float p_smoothing);

	bool get_consider_low_processor_usage_mode() const;
	void set_consider_low_processor_usage_mode(bool p_consider);

	/// Continuous recommended scale (between `min_scale` and `max_scale`).
	float get_current_scale() const;

	/// Current EMA of measured frames per second (0 until the first frame).
	float get_measured_fps() const;

	/// 0 = low, 1 = medium, 2 = high, derived from `get_current_scale()`.
	int get_tier() const;

protected:
	void _process(double p_delta);
	void _quality_changed();

	static void _bind_methods();

private:
	bool enabled = true;
	float target_fps = 60.0f;
	float min_scale = 0.5f;
	float max_scale = 1.0f;
	float smoothing = 0.1f;
	bool consider_low_processor_usage_mode = true;

	float ema_fps = 0.0f;
	float current_scale = 1.0f;
};
