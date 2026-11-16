in vec3 vNormal;
in vec3 vWorldPos;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uLightDir;
uniform int  uChecker;   // 1 = szachownica (dla podłogi)

void main() {
    vec3 base = uColor;
    if (uChecker == 1) {
        vec2 cell = floor(vWorldPos.xz);
        float k = mod(cell.x + cell.y, 2.0);
        base *= mix(1.0, 0.55, k);
    }
    vec3 n = normalize(vNormal);
    float diff = max(dot(n, normalize(uLightDir)), 0.0);
    FragColor = vec4(base * (0.25 + 0.75 * diff), 1.0);
}
