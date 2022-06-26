float dot2( vec2 a, vec2 b ) { return dot( a, b ); }
float dot3( vec3 a, vec3 b ) { return dot( a, b ); }
float dot3( vec3 a, vec4 b ) { return dot( a, b.xyz ); }
float dot3( vec4 a, vec3 b ) { return dot( a.xyz, b ); }
float dot3( vec4 a, vec4 b ) { return dot( a.xyz, b.xyz ); }
float dot4( vec4 a, vec4 b ) { return dot( a, b ); }
float dot4( vec2 a, vec4 b ) { return dot( vec4( a, 0, 1 ), b ); }

vec4 tex2D( sampler2D samp, vec2 texCoords ) { return texture2D(samp, texCoords); }
vec4 idtex2Dproj( sampler2D samp, vec4 texCoords ) { return texture2DProj( samp, texCoords.xyw ); }

float saturate( float v ) { return clamp( v, 0.0, 1.0 ); }
vec2 saturate( vec2 v ) { return clamp( v, 0.0, 1.0 ); }
vec3 saturate( vec3 v ) { return clamp( v, 0.0, 1.0 ); }
vec4 saturate( vec4 v ) { return clamp( v, 0.0, 1.0 ); }

// https://www.gamedev.net/forums/topic/687535-implementing-a-cube-map-lookup-function/
vec3 ComputeSampleCube( const vec3 v)
{
	vec3 vAbs = abs(v);
	float ma;
	vec2 uv;
	float faceIndex;
	
	if(vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
	{
		faceIndex = v.z < 0.0 ? 5.0 : 4.0;
		ma = 0.5 / vAbs.z;
		uv = vec2(v.z < 0.0 ? -v.x : v.x, -v.y);
	}
	else if(vAbs.y >= vAbs.x)
	{
		faceIndex = v.y < 0.0 ? 3.0 : 2.0;
		ma = 0.5 / vAbs.y;
		uv = vec2(v.x, v.y < 0.0 ? -v.z : v.z);
	}
	else
	{
		faceIndex = v.x < 0.0 ? 1.0 : 0.0;
		ma = 0.5 / vAbs.x;
		uv = vec2(v.x < 0.0 ? v.z : -v.z, -v.y);
	}
	return vec3(uv * ma + 0.5, faceIndex);
}

float SampleShadowMapPCF(sampler2D shadowMapAtlas, vec2 uv, float scenedepth, vec4 rpShadowMapInfo) {
	float shadowMapSliceSize = 1.0 / rpShadowMapInfo.z;
	float factor = 0;
	float numSamples = 0;
    for (int y = -1 ; y <= 1 ; y++) {
        for (int x = -1 ; x <= 1 ; x++) {
			float shadowdepth = tex2D(shadowMapAtlas, uv + vec2(x * shadowMapSliceSize, y * shadowMapSliceSize)).r * 3000;
			if(scenedepth <= shadowdepth)
				factor += 1.0;
			
			numSamples++;
		}	
	}
	
	return factor / numSamples;
}

#ifdef ID_SPOTLIGHT
vec2 ShowSpotLightCoords(vec3 vertexPos, vec3 lightPos) {
	vec4 shadowTexCoords;
	vec4 _vertexPos = vec4(vertexPos.x, vertexPos.y, vertexPos.z, 1.0);

	shadowTexCoords.x = dot4(_vertexPos, rpShadowMatrixX);	
	shadowTexCoords.y = dot4(_vertexPos, rpShadowMatrixY);
	shadowTexCoords.z = dot4(_vertexPos, rpShadowMatrixZ);
	shadowTexCoords.w = dot4(_vertexPos, rpShadowMatrixW);

	vec2 initialuv = shadowTexCoords.xy / shadowTexCoords.w;

	return initialuv;
}
#endif

float CalculateShadows(sampler2D atlasLookup, sampler2D shadowMapAtlas, vec3 vertexPos, vec3 lightPos, vec4 ShadowMapInfo) {
	vec3 vertToLight = vertexPos - lightPos;
#ifdef ID_SPOTLIGHT
	vec4 shadowTexCoords;
	vec4 _vertexPos = vec4(vertexPos.x, vertexPos.y, vertexPos.z, 1.0);

	shadowTexCoords.x = dot4(_vertexPos, shadowMatrixX);	
	shadowTexCoords.y = dot4(_vertexPos, shadowMatrixY);
	shadowTexCoords.z = dot4(_vertexPos, shadowMatrixZ);
	shadowTexCoords.w = dot4(_vertexPos, shadowMatrixW);

	vec2 initialuv = shadowTexCoords.xy / shadowTexCoords.w;
	initialuv = initialuv * 0.5 + 0.5;
	float slice = 0;	
#else
	vec3 cubeInfo = ComputeSampleCube(vertToLight);
	vec2 initialuv = cubeInfo.xy;
	float slice = cubeInfo.z;
#endif

	initialuv *= ShadowMapInfo.y;
	
	// Get the slice information(aka were in the atlas are we going to sample from).
	vec2 sliceOffset = texelFetch(atlasLookup, ivec2(ShadowMapInfo.x + slice, 0), 0).xy;
	vec2 uv = initialuv + sliceOffset;
	
	float scenedepth = distance(vertexPos, lightPos);
	return SampleShadowMapPCF(shadowMapAtlas, uv, scenedepth, ShadowMapInfo);
}

float AttenuationSpotLight( float lightDist, float maxLightDist, float falloffPower ) {
	float attenuation = 1.0f - saturate( lightDist / maxLightDist );
	falloffPower = max( falloffPower, 0.0f );
	return pow( attenuation, falloffPower );
}

float AttenuationPointLight( vec3 fragmentInWorldSpace, vec4 curLightPosInWorldSpace, vec4 curLightExtents ) {
	float falloffPower = curLightPosInWorldSpace.w;
	vec3 perAxis = 1.0f - saturate( abs( fragmentInWorldSpace - curLightPosInWorldSpace.xyz ) / curLightExtents.xyz );			
	float attenuation = pow( perAxis.x * perAxis.y * perAxis.z, falloffPower );
	
	return attenuation;
}