#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <vector>
#include <glm/glm_basics.h>
#include <cmath>
#include <iostream>

#include "shader.h"
#include "opengl_funcs.h"

constexpr float WINDOW_HEIGHT = 700.0f;
constexpr float WINDOW_WIDTH  = 1280.0f;

std::vector<GLfloat> vertices =
{
     0.5f,  0.5f, 0.0f,  // Top Right
     0.5f, -0.5f, 0.0f,  // Bottom Right
    -0.5f, -0.5f, 0.0f,  // Bottom Left
    -0.5f,  0.5f, 0.0f   // Top Left
};

std::vector<GLuint> indices =
{
    0, 1, 3,   // First Triangle
    1, 2, 3    // Second Triangle
};

void
windowEvents ( sf::Window& window);

int
main()
{
    ///====================Step One: CREATE A WINDOW===========================================
    sf::Window window( sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "OpenGL" );      //Like this
    window.setFramerateLimit(60);                                                   //Set it's frame rate limit
    std::vector<GLuint> bufferObjectList;

    ///====================Step Two: Set up openGL stuff ===========================================
    glewExperimental = GL_TRUE; //GLEW
    glewInit();

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  //OpenGL
    glEnable(GL_DEPTH_TEST);

    ///====================Step Three: Create the SHADER ===========================================
    Shader shader ( "Source/Shaders/vert.vert", "Source/Shaders/frag.frag");

    ///====================Step Four: Define a Vertex Array Object ===========================================
    GLuint vertexArrayObject;                           //Create variable
    glGenVertexArrays   (1, &vertexArrayObject);        //Generate IDs for VAO
    glBindVertexArray   ( vertexArrayObject);           //Bind the buffer, every VBO after this is bound to this VAO until they are bound
                                                        //After we unbind the VAO
    glFuncs::addBufferObject    (bufferObjectList, vertices, GL_ARRAY_BUFFER);  //Add VBO
    glFuncs::addAttribPointer   (0, 3);                 //Sometimes, we need to set an attib position (aka location in the vertex shader
                                                        //If there is a variable label as "in"

    glFuncs::addBufferObject    (bufferObjectList, indices, GL_ELEMENT_ARRAY_BUFFER);   //Add EBO

    glBindVertexArray(0);                       //Unbind the bufferers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    sf::Clock c;

    ///====================Step four: Start the Loop ===========================================
    while ( window.isOpen() ) {
        float time = c.getElapsedTime().asSeconds();

        ///====================Loop Step One: Clear the window ===========================================
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        ///====================Loop Step Two: Bind a SHADER ===========================================
        shader.useProgram();

        ///====================Loop Step Three: Set up some matrices (GLM) ===========================================
        ///~~~~~~~~~~~~~ Sub Step One: Declare variables ~~~~~~~~~~~~
        ///This MUST be done in the loop and separately from their definitions
                            //We need 3 matrices: projection, view, and model.
        glm::mat4 proj;     //This makes the world appear 3 dimensional via wizardry ( thanks GLM )
        glm::mat4 view;     //This is where the viewer is in the world.
        glm::mat4 model;    //This is where the object appears in the world

        ///~~~~~~~~~~~~ Sub Step Two: Define the matrix variables ~~~~~~~~~~~~
        model = glm::rotate         (model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //Can be defined as either translate, rotate or scale ( or combination!)
        view =  glm::translate      (view,  glm::vec3(0.0f, 0.0f, -3.0f));
        proj =  glm::perspective    (90.0f, WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);

        /// ~~~~~~~~~~~~Sub Step Three (Optional): Multiply the matrices with more matrices ~~~~~~~~~~~~
        glm::mat4 trans;
        trans = glm::translate(trans, glm::vec3(sin(time), 0, 0 ));

        glm::mat4 scale;
        scale = glm::scale(scale, glm::vec3( 3, 2, 0));

        model = scale * trans * model;

        /// ~~~~~~~~~~~~Sub Step Four: Get the locations from the shader into an ID (GL unsigned integer) ~~~~~~~~~~~~
        GLuint projLoc  = glGetUniformLocation(shader.getProgramId(), "projectionMatrix");  //Don't forget to write this stuff
        GLuint viewLoc  = glGetUniformLocation(shader.getProgramId(), "viewMatrix");        //up in the vertex SHADER
        GLuint modelLoc = glGetUniformLocation(shader.getProgramId(), "modelMatrix");

        /// ~~~~~~~~~~~~ Sub Step Five: Upload the matrix data to the shaders  ~~~~~~~~~~~~
        glUniformMatrix4fv ( modelLoc, 1, GL_FALSE, glm::value_ptr ( model ) );
        glUniformMatrix4fv ( viewLoc,  1, GL_FALSE, glm::value_ptr ( view  ) );
        glUniformMatrix4fv ( projLoc,  1, GL_FALSE, glm::value_ptr ( proj  ) );

        ///====================Loop Step Five: Bind the VAO you want to render ===========================================
        glBindVertexArray(vertexArrayObject);

        ///====================Loop Step Six: Draw the VAO (via EBO preferably ===========================================
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0 );

        ///====================Loop Step Seven: Unbind the VAO you want to render ===========================================
        glBindVertexArray(0);

        ///====================Loop Step Five: Unbind the shader that was used ===========================================
        shader.unuseProgram();

        ///====================Loop Step Seven: Display the window, and check for window input ===========================================
        window.display();
        windowEvents(window);
    }
    ///====================Step Five: Delete your VBO's and VAO's ===========================================
    for ( auto& vb : bufferObjectList ) {
        glDeleteBuffers(1, &vb);
    }
    glDeleteVertexArrays ( 1, &vertexArrayObject );

    ///====================Step Six: Exit ===========================================
    return 0;
}

void
windowEvents ( sf::Window& window)
{
    sf::Event e;

    while ( window.pollEvent ( e ) ) {
        if ( e.type == sf::Event::Closed ) {
            window.close();
        }
    }
}
