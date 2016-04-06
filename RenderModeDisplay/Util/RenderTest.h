
#pragma once
#include <string>

#include "Types/Application//Model.h"
#include "Types\ModelManager.h"
#include "Command\CommandBucket.h"
#include "Command\CommandBuilder.h"
#include "Render\RenderContext.h"
#include "Render\RenderQueue.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
using std::cout;
using std::endl;

//std::string squareobjfile =
//"v - 0 0\n"
//"v 1 0 0\n"
//"v 1 1 0\n"
//"v 0 1 0\n"
//"vt 0 0\n"
//"vt 1 0\n"
//"vt 1 1\n"
//"vt 0 1\n"
//"vn 0 0 1\n"
//"vn 0 0 1\n"
//"vn 0 0 1\n"
//"vn 0 0 1\n"
//"f 1/1/1 2/2/2 3/3/3\n"
//"f 1/1/1 3/3/3 4/4/4";


float v[] = {
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};

unsigned int i[] = {
    0, 1, 2,
    0, 2, 3
};

//float vt[] = {
//    0, 0,
//    1, 0,
//    1, 1,
//    0, 1
//};
//
//float vn[] = {
//    0, 0, 1,
//    0, 0, 1,
//    0, 0, 1,
//    0, 0, 1
//};
//
//int i[] = {
//    1, 1, 1,
//    2, 2, 2,
//    3, 3, 3,
//    1, 1, 1,
//    3, 3, 3,
//    4, 4, 4
//};

//
//float onlyVerticesSq[]{
//    -0.9f,-0.9f, 1.0f,
//    0.85f,-0.9f, 1.0f,
//    -0.9f, 0.85f, 1.0f,
//    0.9f,-0.85f, 1.0f,
//    0.9f,0.9f, 1.0f,
//    -0.85f,0.9f, 1.0f 
//};

float tri[]{
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

float triNorm[]{
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
};

float uv[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f
};

unsigned int triIndices[]{
    0, 1, 2
};

char vertexShader[] =
"#version 430 core\n"
"layout(location = 0) in vec4 pos;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"    gl_Position = mvp * pos;\n"
"}";

char fragmentShader[] =
"#version 430 core\n"
"out vec4 fragColor;\n"
"void main() {\n"
"    fragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
"}\n";


int w = 4;
int h = 4;
// build a red RGB texture

unsigned char texData[] = {
    0xff, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0xff, 0x00, 0x00,  0xff, 0x00, 0x00,
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0xff, 0x00, 0x00,  0xff, 0x00, 0x00,
};

// GREEN
unsigned char texData2[] = {
    0x00, 0xff, 0x00,  0x00, 0xff, 0x00,  0x00, 0xff, 0x00,  0x00, 0xff, 0x00,
    0x00, 0xff, 0x00,  0x00, 0xff, 0x00,  0x00, 0xff, 0x00,  0x00, 0xff, 0x00,
    0x00, 0xff, 0x00,  0x00, 0xff, 0x00,  0x00, 0xff, 0x00,  0x00, 0xff, 0x00,
    0x00, 0xff, 0x00,  0x00, 0xff, 0x00,  0x00, 0xff, 0x00,  0x00, 0xff, 0x00,
};


void loadTextFile(char * fname, std::string &loadedFile) {
    std::ifstream file(fname);
    if (!file) {
        cout << "unable to find file!" << endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        loadedFile += line;
        loadedFile.push_back('\n');
    }
    cout << fname << ": file loaded." << endl;
}

class RenderTest {
public:
    RenderTest() {
        renderContext = new RenderContext(1024, 1024, 1024, 1024, 1024);
        cmdBucket = new CommandBucket(100, 1024 * 1024 * 5);
        cmdBuilder = new CommandBuilder(*cmdBucket, *renderContext);
        mgr = new ModelManager(1024 * 30, 1024, cmdBuilder);

    }

    ~RenderTest() {
        delete cmdBuilder;
        delete cmdBucket;
        delete mgr;
        delete renderContext;
    }

    int setupShaders(const char * vertex,const char *frag, bool setupUBO = false) {
        int executed = 0;
        // Load and set shader
        shaderProgramHandle = renderContext->shaderProgramsPool.createObject();
        vertexShaderData.source = vertex;
        fragmentShaderData.source = frag;
        Handle loadShaderCmd = cmdBuilder->buildCreateShaderCommand(vertexShaderData, fragmentShaderData, shaderProgramHandle);
        Handle lightHandle;
        Handle materialHandle;
       
        if (setupUBO) {
            light.location = glm::vec3(1.0f);//glm::vec3(0.0f, 5.0f, 1.0f);
            light.diffuse = glm::vec3(0.1f, 0.1f, 0.1f);//glm::vec3(1.0f, 0.0f, 0.0f);
            light.specular = glm::vec3(1.0f);//glm::vec3(0.0f, 1.0f, 0.0f);
            material.ambient = glm::vec3(0.0f);
            material.diffuse = glm::vec3(0.0f , 1.0f, 0.0f);
            material.specular = glm::vec3(0.0f,1.0f,0.0f);

            lightUboHandle = renderContext->bufferObjectPool.createObject();
            lightUbo = renderContext->bufferObjectPool.get(lightUboHandle);
            materialUboHandle = renderContext->bufferObjectPool.createObject();
            materialUbo = renderContext->bufferObjectPool.get(materialUboHandle);


            lightHandle = cmdBuilder->buildCreateUniformBufferCommand(lightUboHandle, 
                sizeof(GPU::Uniform::LightSource), 
                (void *) &light, 
                GPU::Uniform::defaultLightSourceUniformBlockBinding);
            
            materialHandle = cmdBuilder->buildCreateUniformBufferCommand(materialUboHandle,
                sizeof(GPU::Uniform::Material),
                (void *) &material,
                GPU::Uniform::defaultMaterialUniformBlockBinding);
        }

        renderQueue.submit(loadShaderCmd, CommandKey());
        renderQueue.submit(lightHandle, CommandKey());
        renderQueue.submit(materialHandle, CommandKey());

        int executedStart = executed;
        executed += renderQueue.execute(*cmdBucket, *renderContext);

        GPU::ShaderProgram *shader = renderContext->shaderProgramsPool.get(shaderProgramHandle);
        setShaderCmd = cmdBuilder->buildSetShaderProgramCommand(*shader);


        renderQueue.submit(setShaderCmd, CommandKey());
        executed += renderQueue.execute(*cmdBucket, *renderContext);

        cout << "Executed by shader creation: " << executed - executedStart << endl;

        return executed;
    }

    //void attemptLoadCommands(bool withCamera = false) {
    //    cout << "starting test" << endl;
    //    int executed = 0;
    //    CommandKey key;
    //    executed += setupShaders(vertexShader, fragmentShader);

    //    // load vertices
    //    Handle geometryBuffer = renderContext->bufferObjectPool.createObject();
    //    GPU::GeometryBufferLayout bufferLayout;

    //    vaoHandle = renderContext->vaoPool.createObject();
    //    Handle vaoParentCommand = cmdBuilder->buildInitializeAndSetVertexArrayCommand(vaoHandle);

    //    Handle loadCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(
    //        SystemBuffer((void*)&tri, sizeof(tri)),  // two triangles
    //        geometryBuffer,
    //        GPU::ShaderAttributeBinding::VERTICES,
    //        bufferLayout,
    //        vaoParentCommand);
    //    renderQueue.submit(&vaoParentCommand, &key, 1);

    //    executed += renderQueue.execute(*cmdBucket, *renderContext);

    //    cout << "commands executed: " << executed << endl;
    //    cout << "qsize: " << renderQueue.numCommands() << endl;

    //    GPU::DrawContext drawContext;
    //    drawContext.indexOffset = 0;
    //    drawContext.numElements = 3;
    //    if (withCamera) {
    //        glm::mat4 p = glm::perspective(glm::radians(90.0f), 640.0f / 480.0f, 0.1f, 100.0f);
    //        glm::mat4 v = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //        m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));

    //        GPU::ShaderProgram *shader = renderContext->shaderProgramsPool.get(shaderProgramHandle);
    //        drawCmd = cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.mvp, glm::value_ptr(mvp), 1);
    //        cmdBuilder->buildDrawArraysCommandWithParent(*renderContext->vaoPool.get(vaoHandle), drawContext, drawCmd);
    //    }
    //    else {
    //        drawCmd = cmdBuilder->buildDrawArraysCommand(*renderContext->vaoPool.get(vaoHandle), drawContext);
    //    }
    //}

    //void loadIndexCommands() {
    //    cout << "starting test" << endl;
    //    int executed = 0;
    //    CommandKey key;
    //    executed += setupShaders(vertexShader, fragmentShader);

    //    // load vertices
    //    Handle geometryBufferHandle = renderContext->bufferObjectPool.createObject();
    //    Handle indexBufferHandle = renderContext->bufferObjectPool.createObject();
    //    GPU::GeometryBufferLayout bufferLayout;

    //    vaoHandle = renderContext->vaoPool.createObject();
    //    Handle vaoParentCommand = cmdBuilder->buildInitializeAndSetVertexArrayCommand(vaoHandle);
    //    Handle loadIndexCmd = cmdBuilder->buildLoadIndexArrayCommandWithParent(SystemBuffer((void*)&i, sizeof(i)),
    //        indexBufferHandle,
    //        vaoParentCommand);
    //    Handle loadVertexCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(
    //        SystemBuffer((void*)&v, sizeof(v)),  // two triangles
    //        geometryBufferHandle,
    //        GPU::ShaderAttributeBinding::VERTICES,
    //        bufferLayout,
    //        vaoParentCommand);
    //    renderQueue.submit(&vaoParentCommand, &key, 1);

    //    executed += renderQueue.execute(*cmdBucket, *renderContext);

    //    cout << "commands executed: " << executed << endl;
    //    cout << "qsize: " << renderQueue.numCommands() << endl;

    //    GPU::DrawContext drawContext;
    //    drawContext.indexOffset = 0;
    //    drawContext.numElements = 6;
    //    drawCmd = cmdBuilder->buildDrawIndexedCommand(*renderContext->vaoPool.get(vaoHandle), drawContext);
    //}

    // todo move this to model load as part of material.

    struct ModelDrawContext {
        Model * model;


        Handle texBufferHandle;
        GPU::TextureBufferObject *texBuffer; // convenience ptr to actual buffer


        GPU::Uniform::Material material;

        glm::mat4 m;
        glm::mat4 mvp;
        glm::mat4 mv;

        Handle staticDrawCommand;
    };

    void submitLoadTexture(ModelDrawContext *drawContext, void *texData, size_t size, int width, int height) {
        drawContext->texBufferHandle = renderContext->bufferObjectPool.createObject();
        drawContext->texBuffer = renderContext->bufferObjectPool.get(drawContext->texBufferHandle);

        GPU::TextureBufferLayout textureLayout;
        textureLayout.width = width;
        textureLayout.height = height;
        textureLayout.textureFormat = GPU::TextureFormat::RGB;

        Handle loadTexCmd = cmdBuilder->buildLoadTextureCommand(SystemBuffer(texData, size),
            textureLayout,
            drawContext->texBufferHandle);
        renderQueue.submit(loadTexCmd, CommandKey());
    }

    ModelDrawContext shape1;
    ModelDrawContext shape2;

    void buildStaticDrawCommand(ModelDrawContext &drawContext, GPU::ShaderProgram *shaderProgram) {
        // Camera transformations
        drawContext.mvp = p * v * drawContext.m;
        drawContext.mv = v * drawContext.m;

        GPU::ShaderProgram *shader = renderContext->shaderProgramsPool.get(shaderProgramHandle);
        Handle cmd = cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.mvp, glm::value_ptr(drawContext.mvp), 1);
        cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.mv, glm::value_ptr(drawContext.mv), 1, cmd);
        cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.v, glm::value_ptr(v), 1, cmd);
        cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.m, glm::value_ptr(drawContext.m), 1, cmd);

        cmdBuilder->buildUpdateUniformBufferCommand(*materialUbo, (void*)&drawContext.material, sizeof(GPU::Uniform::Material), 0, cmd);
        //cmdBuilder->buildUpdateUniformBufferCommand(*lightUbo, (void*)&light, sizeof(GPU::Uniform::LightSource), 0, cmd); // remove?
        cmdBuilder->buildSetTexturecommand(*drawContext.texBuffer, 0, shader->uniformLocations.diffuseTexture, cmd);
        for (int i = 0; i < drawContext.model->numMeshes; i++) {
            Mesh &mesh = drawContext.model->meshes[i];
            cmdBuilder->buildDrawIndexedCommand(*renderContext->bufferObjectPool.get(drawContext.model->vao), mesh.drawContext, cmd);
        }

        drawContext.staticDrawCommand = cmd;
    }

    void doubleModel() {
        // Setup shader
        std::string vertTex;
        std::string fragTex;
        loadTextFile("Util/Lighting.vert", vertTex);
        loadTextFile("Util/Lighting.frag", fragTex);
        setupShaders(vertTex.c_str(), fragTex.c_str(), true);
        GPU::ShaderProgram *shaderProgram = renderContext->shaderProgramsPool.get(shaderProgramHandle);



        shape1.model = mgr->loadModelGeometry(std::string("dodecahedron.obj"), *renderContext);
        shape1.m = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, -2.0f));
        submitLoadTexture(&shape1, (void *)&texData, sizeof(texData), w, h);    // SPLOTCHY RED


        shape2.model = mgr->loadModelGeometry(std::string("cube.obj"), *renderContext);
        shape2.m = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, -2.0f));
        submitLoadTexture(&shape2, (void *)&texData2, sizeof(texData), w, h);   // GREEN



        // submit model loads
        mgr->submitModelLoadCommands(shape1.model->modelGeometryLoadDataHandle, renderQueue, *renderContext);
        mgr->submitModelLoadCommands(shape2.model->modelGeometryLoadDataHandle, renderQueue, *renderContext);
        int exec = renderQueue.execute(*cmdBucket, *renderContext);


        p = glm::perspective(glm::radians(90.0f), 640.0f / 480.0f, 0.1f, 100.0f);
        v = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        buildStaticDrawCommand(shape1, shaderProgram);
        buildStaticDrawCommand(shape2, shaderProgram);
    }

    void drawShapes() {
        renderQueue.submit(shape1.staticDrawCommand, CommandKey());
        renderQueue.submit(shape2.staticDrawCommand, CommandKey());

        renderQueue.execute(*cmdBucket, *renderContext);
    }


    Model * model;
    void modelLoad() {
        model = mgr->loadModelGeometry(std::string("dodecahedron.obj"), *renderContext);
        ModelGeometryLoadData *geometryData = renderContext->modelGeometryPool.get(model->modelGeometryLoadDataHandle);

        // Setup shader
        std::string vertTex;
        std::string fragTex;
        loadTextFile("Util/BasicCamera.vert", vertTex);
        loadTextFile("Util/Basic.frag", fragTex);
        setupShaders(vertTex.c_str(), fragTex.c_str(), true);

        // Load model into gpu
        mgr->submitModelLoadCommands(model->modelGeometryLoadDataHandle, renderQueue, *renderContext);

        Handle texBuffer = renderContext->bufferObjectPool.createObject();
        GPU::TextureBufferLayout textureLayout;
        textureLayout.width = w;
        textureLayout.height = h;
        textureLayout.textureFormat = GPU::TextureFormat::RGB;
        Handle loadTexCmd = cmdBuilder->buildLoadTextureCommand(SystemBuffer((void*)&texData, sizeof(texData)),
            textureLayout,
            texBuffer);
        renderQueue.submit(loadTexCmd, CommandKey());
        int exec = renderQueue.execute(*cmdBucket, *renderContext);
        cout << "executed " << exec << " load cmds for model!" << endl;

        // Camera transformations
        p = glm::perspective(glm::radians(90.0f), 640.0f / 480.0f, 0.1f, 100.0f);
        m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
        v = glm::lookAt(glm::vec3(-1.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mvp = p * v * m;
        mv = v * m;

        GPU::ShaderProgram *shader = renderContext->shaderProgramsPool.get(shaderProgramHandle);
        drawCmd = cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.mvp, glm::value_ptr(mvp), 1);
        cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.mv, glm::value_ptr(mv), 1, drawCmd);
        cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.v, glm::value_ptr(v), 1, drawCmd);
        cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.m, glm::value_ptr(m), 1, drawCmd);

        cmdBuilder->buildUpdateUniformBufferCommand(*materialUbo, (void*)&material, sizeof(GPU::Uniform::Material), 0, drawCmd);
        cmdBuilder->buildUpdateUniformBufferCommand(*lightUbo, (void*)&light, sizeof(GPU::Uniform::LightSource), 0, drawCmd);


        for (int i = 0; i < model->numMeshes; i++) {
            Mesh &m = model->meshes[i];
            cmdBuilder->buildDrawIndexedCommand(*renderContext->bufferObjectPool.get(model->vao), m.drawContext, drawCmd);
        }
    }

    void texLoad(bool withLight = false) {
        int executed = 0;

        std::string vertTex;
        std::string fragTex;
        loadTextFile("Util/Lighting.vert", vertTex);
        loadTextFile("Util/Lighting.frag", fragTex);
        executed += setupShaders(vertTex.c_str(), fragTex.c_str(), true);

        // load vertices
        Handle geometryBuffer = renderContext->bufferObjectPool.createObject();
        GPU::GeometryBufferLayout bufferLayout;

        vaoHandle = renderContext->bufferObjectPool.createObject();
        Handle vaoParentCommand = cmdBuilder->buildInitializeAndSetVertexArrayCommand(vaoHandle);

        Handle loadCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(
            SystemBuffer((void*)&tri, sizeof(tri)),  
            geometryBuffer,
            GPU::ShaderAttributeBinding::VERTICES,
            bufferLayout,
            vaoParentCommand);

        Handle uvBuffer = renderContext->bufferObjectPool.createObject();
        GPU::GeometryBufferLayout uvLayout;
        uvLayout.numComponents = 2;
        Handle loadUvCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(
            SystemBuffer((void*)&uv, sizeof(uv)),
            uvBuffer,
            GPU::ShaderAttributeBinding::UV,
            uvLayout,
            vaoParentCommand);

        Handle normalBuffer = renderContext->bufferObjectPool.createObject();
        GPU::GeometryBufferLayout normalLayout;
        normalLayout.numComponents = 3;
        Handle loadNormalsCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(
            SystemBuffer((void *)&triNorm, sizeof(triNorm)),
            normalBuffer,
            GPU::ShaderAttributeBinding::NORMALS,
            normalLayout,
            vaoParentCommand);

        Handle texBuffer = renderContext->bufferObjectPool.createObject();
        textureBuffer = renderContext->bufferObjectPool.get(texBuffer);
        GPU::TextureBufferLayout textureLayout;
        textureLayout.width = w;
        textureLayout.height = h;
        textureLayout.textureFormat = GPU::TextureFormat::RGB;
        Handle loadTexCmd = cmdBuilder->buildLoadTextureCommand(SystemBuffer((void*)&texData, sizeof(texData)),
            textureLayout, 
            texBuffer, 
            vaoParentCommand);


        renderQueue.submit(vaoParentCommand, CommandKey());

        executed += renderQueue.execute(*cmdBucket, *renderContext);


        cout << "commands executed for load: " << executed << endl;
        cout << "qsize: " << renderQueue.numCommands() << endl;

        GPU::DrawContext drawContext;
        drawContext.indexOffset = 0;
        drawContext.numElements = 3;

        // TODO move this junk to just be single commands rather than growing this list..

        // Camera transformations
        p = glm::perspective(glm::radians(90.0f), 640.0f / 480.0f, 0.1f, 100.0f);
        m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
        v = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mvp = p * v * m;
        mv = v * m;

        GPU::ShaderProgram *shader = renderContext->shaderProgramsPool.get(shaderProgramHandle);
        drawCmd = cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.mvp, glm::value_ptr(mvp), 1);
        cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.mv, glm::value_ptr(mv), 1, drawCmd);
        cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.v, glm::value_ptr(v), 1, drawCmd);
        cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.m, glm::value_ptr(m), 1, drawCmd);

        cmdBuilder->buildUpdateUniformBufferCommand(*materialUbo, (void*)&material, sizeof(GPU::Uniform::Material), 0, drawCmd);
        cmdBuilder->buildUpdateUniformBufferCommand(*lightUbo, (void*)&light, sizeof(GPU::Uniform::LightSource), 0, drawCmd);
        cmdBuilder->buildSetTexturecommand(*textureBuffer, 0, shader->uniformLocations.diffuseTexture, drawCmd);
        cmdBuilder->buildDrawArraysCommandWithParent(*renderContext->bufferObjectPool.get(vaoHandle), drawContext, drawCmd);
    }


    void draw() {
        renderQueue.submit(drawCmd, CommandKey());
        renderQueue.execute(*cmdBucket, *renderContext);
    }

    Handle drawCmd;

    // shader commands
    Handle shaderProgramHandle;
    GPU::ShaderData vertexShaderData;
    GPU::ShaderData fragmentShaderData;
    Handle setShaderCmd;

    Handle lightUboHandle;
    GPU::UniformBufferObject* lightUbo;
    Handle materialUboHandle;
    GPU::UniformBufferObject* materialUbo;

    GPU::Uniform::LightSource light;
    GPU::Uniform::Material material;
    GPU::TextureBufferObject *textureBuffer;


    Handle vaoHandle;
    ModelManager *mgr;
    CommandBucket *cmdBucket;
    CommandBuilder *cmdBuilder;
    RenderContext *renderContext;
    RenderQueue renderQueue;

    glm::mat4 p;
    glm::mat4 m;
    glm::mat4 v;
    glm::mat4 mv;
    glm::mat4 mvp;
};
