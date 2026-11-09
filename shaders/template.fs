#version 330 core

uniform sampler2D tex_2d;
uniform vec4 color;

in float vert_speed;

out vec4 out_col;

void main() {
    float t = clamp(vert_speed * 0.5, 0.0, 1.0);

    vec3 dark_color = color.rgb * 0.2;
    vec3 bright_color = color.rgb + vec3(0.4);

    float greyscale_factor = dot(color.rgb, vec3(0.21, 0.71, 0.07));
    vec3 semi_greyscale = mix(vec3(greyscale_factor), dark_color, 0.7f);

    vec3 animated_color = mix(semi_greyscale, bright_color, t);
    out_col = vec4(animated_color, 1.0);
}
