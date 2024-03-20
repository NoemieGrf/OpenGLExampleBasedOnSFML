#pragma once

enum class VertexLayout
{
	PositionTexcoord,
	PositionNormalTexcoord
};



template<VertexLayout Layout>
class Object
{
public:
	Object(unsigned int dataSizeInBytes, float* data)
    {
        /* [VBO] Create vertex buffer */
        ::glGenBuffers(1, &_vertexBufferHandle);
        ::glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferHandle);
        ::glBufferData(GL_ARRAY_BUFFER, dataSizeInBytes, data, GL_STATIC_DRAW);

        /* [VAO] Generate vertex array for plane and bind it */
        ::glGenVertexArrays(1, &_vertenArrayHanle);
        ::glBindVertexArray(_vertenArrayHanle);


        /* [VAO] Open vertex attrib */
        if constexpr (Layout == VertexLayout::PositionTexcoord)
        {
            ::glEnableVertexAttribArray(0);
            ::glEnableVertexAttribArray(1);
            ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
            ::glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

            _triangleNum = dataSizeInBytes / 5;
        }
        else if constexpr (Layout == VertexLayout::PositionNormalTexcoord)
        {
            ::glEnableVertexAttribArray(0);
            ::glEnableVertexAttribArray(1);
            ::glEnableVertexAttribArray(2);
            ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
            ::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
            ::glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

            _triangleNum = dataSizeInBytes / 8;
        }
    }

public:
    unsigned int GetVertexBufferHandle()
    {
        return _vertexBufferHandle;
    }

	unsigned int GetVertexArrayHandle()
    {
        return _vertenArrayHanle;
    }

    unsigned int GetTriangleNum()
    {
        return _triangleNum;
    }

private:
    unsigned int _triangleNum;
	unsigned int _vertexBufferHandle;
	unsigned int _vertenArrayHanle;
};

