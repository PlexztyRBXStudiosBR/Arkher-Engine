shader_type spatial;

// Arkher Terrain v0.1 — splat de 4 materiais por vértice.
// Os pesos dos 4 materiais vêm do atributo COLOR (R,G,B,A).

uniform sampler2D albedo_a : source_color;
uniform sampler2D albedo_b : source_color;
uniform sampler2D albedo_c : source_color;
uniform sampler2D albedo_d : source_color;

uniform float roughness_a : hint_range(0.0, 1.0) = 0.95;
uniform float roughness_b : hint_range(0.0, 1.0) = 0.9;
uniform float roughness_c : hint_range(0.0, 1.0) = 0.85;
uniform float roughness_d : hint_range(0.0, 1.0) = 0.8;

uniform float metallic_a : hint_range(0.0, 1.0) = 0.0;
uniform float metallic_b : hint_range(0.0, 1.0) = 0.0;
uniform float metallic_c : hint_range(0.0, 1.0) = 0.0;
uniform float metallic_d : hint_range(0.0, 1.0) = 0.0;

uniform float uv_scale : hint_range(0.01, 128.0) = 16.0;

varying vec4 v_w;
varying vec2 v_uv;

void vertex() {
	v_w = COLOR;
	v_uv = UV * uv_scale;
}

void fragment() {
	float s = max(v_w.r + v_w.g + v_w.b + v_w.a, 1e-5);
	float wr = v_w.r / s;
	float wg = v_w.g / s;
	float wb = v_w.b / s;
	float wa = v_w.a / s;

	vec3 albedo = texture(albedo_a, v_uv).rgb * wr
			+ texture(albedo_b, v_uv).rgb * wg
			+ texture(albedo_c, v_uv).rgb * wb
			+ texture(albedo_d, v_uv).rgb * wa;

	ALBEDO = albedo;
	ROUGHNESS = roughness_a * wr + roughness_b * wg + roughness_c * wb + roughness_d * wa;
	METALLIC = metallic_a * wr + metallic_b * wg + metallic_c * wb + metallic_d * wa;
}
