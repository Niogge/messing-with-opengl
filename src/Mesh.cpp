#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,std::vector<Texture> textures)
    :m_vertices(vertices), m_indices(indices), m_textures(textures)
{
    setupMesh();
}

void Mesh::setupMesh(){
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1,&m_VBO);
    glGenBuffers(1,&m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() *sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); //pos
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1); //Normal
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,Normal));
    
    glEnableVertexAttribArray(2); //TexCoords
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,TexCoords));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader){
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    for (unsigned int  i = 0; i < m_textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 +i);

        std::string number;
        std::string name = m_textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);

        shader.setInt(("material."+name+number), i);
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);        
    glActiveTexture(GL_TEXTURE0);
}