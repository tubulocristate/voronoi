#version 330

//precision mediump float;

uniform vec2 resolution;
uniform vec2 seed;
uniform vec4 color;
uniform int radius;

out vec4 out_color;

void main(void) {
    if ( (length(gl_FragCoord.xy - seed)) > radius) {
        gl_FragDepth = length(gl_FragCoord.xy - seed) / length(resolution); // euclidean
        //gl_FragDepth = (abs(gl_FragCoord.x - seed.x) + abs(gl_FragCoord.y - seed.y))/ length(resolution); // manhattan
        //gl_FragDepth = max(abs(gl_FragCoord.x - seed.x), abs(gl_FragCoord.y - seed.y)) / length(resolution); // chebyshev

        //float p = 2;
        //gl_FragDepth = pow(pow(abs(gl_FragCoord.x - seed.x), p) + pow(abs(gl_FragCoord.y - seed.y), p), 1/p) / length(resolution); // minkowski

        out_color = color;
    } else {
        gl_FragDepth = 0.0f;
        out_color = vec4(0.18f, 0.18f, 0.18f, 1.0f);
    }
}
