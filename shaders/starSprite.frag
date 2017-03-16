#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 resolution;

float meta(vec2 uv, vec2 center){
	return 1.0 / ((center.x-uv.x)*(center.x-uv.x) + (center.y-uv.y)*(center.y-uv.y));
}
	
void main( void ) 
{

	vec2 uv = ( gl_FragCoord.xy / resolution.xy ) * 2.0 - 1.0;
	uv.x *= resolution.x / resolution.y;

	vec3 finalColor3 = vec3( 1.0, 0.588, 0.235 );
	float multiplier = 0.25+0.05*sin(time*0.967);
	
	vec2 b1 = vec2(0.0, 0.0);
	float meta1 = meta(uv, b1) * multiplier;
	
	vec3 color1 = finalColor3*meta1;
	
	gl_FragColor = vec4(color1, 1.0);
}
