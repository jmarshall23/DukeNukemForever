float dot2( vec2 a, vec2 b ) { return dot( a, b ); }
float dot3( vec3 a, vec3 b ) { return dot( a, b ); }
float dot3( vec3 a, vec4 b ) { return dot( a, b.xyz ); }
float dot3( vec4 a, vec3 b ) { return dot( a.xyz, b ); }
float dot3( vec4 a, vec4 b ) { return dot( a.xyz, b.xyz ); }
float dot4( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4( vec2 a, vec4 b ) { return dot( vec4( a, 0, 1 ), b ); }

vec4 tex2D( sampler2D samp, vec2 texCoords ) { return texture2D(samp, texCoords); }
vec4 idtex2Dproj( sampler2D samp, vec4 texCoords ) { return texture2DProj( samp, texCoords.xyw ); }
