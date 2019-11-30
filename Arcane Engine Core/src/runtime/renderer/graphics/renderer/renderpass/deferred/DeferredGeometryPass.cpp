#include "pch.h"
#include "DeferredGeometryPass.h"

#include <editor/utils/loaders/ShaderLoader.h>
#include <editor/Window.h>

#include <runtime/renderer/graphics/renderer/ModelRenderer.h>
#include <runtime/renderer/terrain/Terrain.h>

namespace arcane {

	DeferredGeometryPass::DeferredGeometryPass(Scene3D *scene) : RenderPass(scene), m_AllocatedGBuffer(true) {
		m_ModelShader = editor::ShaderLoader::loadShader("src/runtime/renderer/shaders/deferred/PBR_Model_GeometryPass.glsl");
		m_TerrainShader = editor::ShaderLoader::loadShader("src/runtime/renderer/shaders/deferred/PBR_Terrain_GeometryPass.glsl");

		m_GBuffer = new GBuffer(editor::Window::getResolutionWidth(), editor::Window::getResolutionHeight());
	}

	DeferredGeometryPass::DeferredGeometryPass(Scene3D *scene, GBuffer *customGBuffer) : RenderPass(scene), m_AllocatedGBuffer(false), m_GBuffer(customGBuffer) {
		m_ModelShader = editor::ShaderLoader::loadShader("src/runtime/renderer/shaders/deferred/PBR_Model_GeometryPass.glsl");
		m_TerrainShader = editor::ShaderLoader::loadShader("src/runtime/renderer/shaders/deferred/PBR_Terrain_GeometryPass.glsl");
	}

	DeferredGeometryPass::~DeferredGeometryPass() {
		if (m_AllocatedGBuffer) {
			delete m_GBuffer;
		}
	}

	GeometryPassOutput DeferredGeometryPass::executeGeometryPass(ICamera *camera, bool renderOnlyStatic) {
		glViewport(0, 0, m_GBuffer->getWidth(), m_GBuffer->getHeight());
		m_GBuffer->bind();
		m_GBuffer->clear();
		m_GLCache->setBlend(false);
		m_GLCache->setMultisample(false);

		// Setup initial stencil state
		m_GLCache->setStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		m_GLCache->setStencilWriteMask(0x00);
		m_GLCache->setStencilTest(true);

		// Setup
		ModelRenderer *modelRenderer = m_ActiveScene->getModelRenderer();
		Terrain *terrain = m_ActiveScene->getTerrain();

		m_GLCache->switchShader(m_ModelShader);
		m_ModelShader->setUniform("viewPos", camera->getPosition());
		m_ModelShader->setUniform("view", camera->getViewMatrix());
		m_ModelShader->setUniform("projection", camera->getProjectionMatrix());

		// Setup model renderer for opaque objects only
		if (renderOnlyStatic) {
			m_ActiveScene->addOpaqueStaticModelsToRenderer();
		}
		else {
			m_ActiveScene->addOpaqueModelsToRenderer();
		}

		// Render opaque objects (use stencil to denote models for the deferred lighting pass)
		m_GLCache->setStencilWriteMask(0xFF);
		m_GLCache->setStencilFunc(GL_ALWAYS, DeferredStencilValue::ModelStencilValue, 0xFF);
		modelRenderer->setupOpaqueRenderState();
		modelRenderer->flushOpaque(m_ModelShader, MaterialRequired);
		m_GLCache->setStencilWriteMask(0x00);

		// Setup terrain information
		m_GLCache->switchShader(m_TerrainShader);
		m_TerrainShader->setUniform("view", camera->getViewMatrix());
		m_TerrainShader->setUniform("projection", camera->getProjectionMatrix());

		// Render the terrain (use stencil to denote the terrain for the deferred lighting pass)
		m_GLCache->setStencilWriteMask(0xFF);
		m_GLCache->setStencilFunc(GL_ALWAYS, DeferredStencilValue::TerrainStencilValue, 0xFF);
		terrain->Draw(m_TerrainShader, MaterialRequired);
		m_GLCache->setStencilWriteMask(0x00);


		// Reset state
		m_GLCache->setStencilTest(false);

		// Render pass output
		GeometryPassOutput passOutput;
		passOutput.outputGBuffer = m_GBuffer;
		return passOutput;
	}
}