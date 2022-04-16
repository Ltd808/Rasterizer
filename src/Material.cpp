#include "Material.h"

Material::Material(Shader* shader, Texture* albedo, Texture* normal, Texture* metallic, Texture* roughness, bool isPBR, bool isRefractive)
{
	this->shader = shader;
    this->albedo = albedo;
    this->normal = normal;
    this->metallic = metallic;
    this->roughness = roughness;
    this->isPBR = isPBR;
    this->isRefractive = isRefractive;
}

void Material::PrepareMaterial(glm::mat4x4 model, glm::mat4x4 view, glm::mat4x4 projection, glm::vec3 position, Sky* sky, GLuint shadowMap)
{
	// Activate shader program
	shader->Use();

	// Set uniforms
	shader->SetMat4("model", model);
	shader->SetMat4("view", view);
	shader->SetMat4("projection", projection); // dont need to do this every frame

    shader->SetVec3("camPos", position);

    // refractive doesnt need this for now
    if(!isRefractive)
    { 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, albedo->ID);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, roughness->ID);
    }
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal->ID);

    // PBR specific
    if (isPBR)
    {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, metallic->ID);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sky->GetIrradianceMap());
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_CUBE_MAP, sky->GetConvolvedSpecularMap());
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, sky->GetBRDFLookUpTexture());

        //IBL
        //shader->SetInt("totalMipLevels", sky->GetTotalMipLevels()); 
    }
    else
    {
        shader->SetFloat("shininess", 16);
    }

    // Set shadow map
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
}