#version 400 core

uniform vec4 		objectColour;
uniform sampler2D 	mainTex;
uniform sampler2DShadow shadowTex;
uniform sampler2D	paintMask;

uniform vec3	lightPos;
uniform float	lightRadius;
uniform vec4	lightColour;

uniform vec3	cameraPos;

uniform bool hasTexture;
uniform bool hasMask;
uniform bool hasFlag;

uniform vec3 paintCentre;
uniform sampler2D diffuseTex;//dral
uniform sampler2D noiseTex;//dral
uniform float time;//dral

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	vec4 shadowProj;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColor;

void main(void)
{
	float shadow = 1.0; // New !
	
	if( IN.shadowProj.w > 0.0) { // New !
		shadow = textureProj ( shadowTex , IN.shadowProj ) * 0.5f;
	}

	vec3  incident = normalize ( lightPos - IN.worldPos );
	float lambert  = max (0.0 , dot ( incident , IN.normal )) * 0.9; 
	
	vec3 viewDir = normalize ( cameraPos - IN . worldPos );
	vec3 halfDir = normalize ( incident + viewDir );

	float rFactor = max (0.0 , dot ( halfDir , IN.normal ));
	float sFactor = pow ( rFactor , 80.0 );
	
	vec4 albedo = IN.colour;
	
	if(hasTexture) {
		albedo.rgb *= texture(mainTex, IN.texCoord).rgb;
	}

	if(hasMask) {
		vec3 maskAlbedo = texture(paintMask, IN.texCoord).rgb;
		if (maskAlbedo != vec3(0, 0, 0)) {
			albedo.rgb = maskAlbedo;
		}
	}

	
		
	

	albedo.rgb = pow(albedo.rgb, vec3(2.2));
	
	fragColor.rgb = albedo.rgb * 0.05f; //ambient
	
	fragColor.rgb += albedo.rgb * lightColour.rgb * lambert * shadow; //diffuse light
	
	fragColor.rgb += lightColour.rgb * sFactor * shadow; //specular light
	
	fragColor.rgb = pow(fragColor.rgb, vec3(1.0 / 2.2f));
	
	fragColor.a = 1;

	fragColor = texture (diffuseTex,vec2(1.0 - IN.texCoord.x, 0.2 - IN.texCoord.y));

	vec4 n = texture (noiseTex,vec2(0.5 - IN.texCoord.x, 0.3 - IN.texCoord.y));
    if(n.r > fract(time))
    {
        fragColor = vec4(0.0,0.0,1.0,1.0);
    }
    else
    {
        discard;
    }

}