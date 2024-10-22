/*
vec3 CMRmap(float t) {
    const vec3 c0 = vec3(-0.046981,0.001239,0.005501);
    const vec3 c1 = vec3(4.080583,1.192717,3.049337);
    const vec3 c2 = vec3(-38.877409,1.524425,20.200215);
    const vec3 c3 = vec3(189.038452,-32.746447,-140.774611);
    const vec3 c4 = vec3(-382.197327,95.587531,270.024592);
    const vec3 c5 = vec3(339.891791,-100.379096,-212.471161);
    const vec3 c6 = vec3(-110.928480,35.828481,60.985694);
    return c0+t*(c1+t*(c2+t*(c3+t*(c4+t*(c5+t*c6)))));
}
source: https://www.shadertoy.com/view/Nd3fR2
*/

namespace util {
	float3 CMRMapFloat(float t) {
		t = min(1.f, t);
		const float3 c0 = float3(-0.046981, 0.001239, 0.005501);
		const float3 c1 = float3(4.080583, 1.192717, 3.049337);
		const float3 c2 = float3(-38.877409, 1.524425, 20.200215);
		const float3 c3 = float3(189.038452, -32.746447, -140.774611);
		const float3 c4 = float3(-382.197327, 95.587531, 270.024592);
		const float3 c5 = float3(339.891791, -100.379096, -212.471161);
		const float3 c6 = float3(-110.928480, 35.828481, 60.985694);
		return c0 + t * (c1 + t * (c2 + t * (c3 + t * (c4 + t * (c5 + t * c6)))));
	}
}