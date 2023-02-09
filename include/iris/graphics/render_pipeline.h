////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <vector>

#include "graphics/material_manager.h"
#include "graphics/mesh_manager.h"
#include "graphics/render_command.h"
#include "graphics/render_graph/render_graph.h"
#include "graphics/render_pass.h"
#include "graphics/render_target_manager.h"
#include "graphics/scene.h"
#include "graphics/single_entity.h"

namespace iris
{

/**
 * This class encapsulates all the logic and machinery of rendering. It creates and manages the primitives a user needs
 * to build their desired rendered output.
 *
 * Once a RenderPipeline is constructed it can be used to create scenes, render passes and render graphs. These can all
 * be assembled how the user wants. The RenderPipeline is then handed over to the Window, where it will be converted
 * into render commands.
 */
class RenderPipeline
{
  public:
    /**
     * Create a new RenderPipeline.
     *
     * @param material_manager
     *   Material manager object.
     *
     * @param mesh_manager
     *   Mesh manager object.
     *
     * @param render_target_manager
     *   Render target manager object.
     *
     * @param width
     *   Width of final render output.
     *
     * @param height
     *   Height of final render output.
     */
    RenderPipeline(
        MaterialManager &material_manager,
        MeshManager &mesh_manager,
        RenderTargetManager &render_target_manager,
        std::uint32_t width,
        std::uint32_t height);
    ~RenderPipeline();

    RenderPipeline(const RenderPipeline &) = delete;
    RenderPipeline &operator=(const RenderPipeline &) = delete;
    RenderPipeline(RenderPipeline &&) = default;
    RenderPipeline &operator=(RenderPipeline &&) = default;

    /**
     * Create a new scene.
     *
     * If the scene is modified after the RenderPipeline has been handed over to the Window any changes will be updated
     * on the next render call.
     *
     * @returns
     *   Pointer to Scene.
     */
    Scene *create_scene();

    /**
     * Create a RenderGraph for use with this pipeline.
     *
     * @returns
     *   Pointer to the newly created RenderGraph.
     */
    RenderGraph *create_render_graph();

    /**
     * Create a new render pass for a scene.
     *
     * @param scene
     *   Scene to create render pass for. The Scene must have been created by this RenderPipeline.
     *
     * @returns
     *   Pointer to RenderPass.
     */
    RenderPass *create_render_pass(Scene *scene);

    /**
     * This method adds additional passes as needed (e.g. shadow passes, SSAO) and then creates a collection of
     * RenderCommand objects, which can then be executed by a renderer.
     *
     * Note this should only be called internally by the engine, calling it manually may produce unexpected results.
     *
     * @returns
     *   Collection of RenderCommand objects representing the full pipeline.
     */
    std::vector<RenderCommand> build();

    /**
     * Rebuilds the RenderCommands as if build() was called but doesn't add additional passes.
     *
     * Note this should only be called internally by the engine, calling it manually may produce unexpected results.
     *
     * @returns
     *   Collection of RenderCommand objects representing the full pipeline.
     */
    std::vector<RenderCommand> rebuild();

    /**
     * Get collection of all RenderPass objects in this pipeline.
     *
     * @returns
     *   Collection of RenderPass objects.
     */
    std::vector<RenderPass *> render_passes() const;

    /**
     * Check if a created object has been mutated.
     *
     * @returns
     *   True if a created object has been mutated, false otherwise.
     */
    bool is_dirty() const;

    /**
     * Reset the dirty state to false.
     *
     * Note this should only be called internally by the engine, calling it manually may produce unexpected results.
     */
    void clear_dirty_bit();

    /**
     * Get scene buy index.
     *
     * @param index.
     *   Index of scene, scenes are created at index 0 and increase monotonically.
     */
    Scene *scene(std::size_t index) const;

  private:
    RenderPass *create_engine_render_pass(Scene *scene);

    /**
     * Add a new pass to collection of passes. This creates a new scene, camera and render target as well as using a
     * callback to allow the caller to set the render graph for the scene.
     *
     * The output from the last pass on input is propagated to the new pass.
     *
     * @param render_passes
     *   Collection to add new pass to.
     *
     * @param create_render_graph_callback
     *   Callback to create the render graph for the new scene.
     *
     * @returns
     *   Target for new pass.
     */
    const RenderTarget *add_pass(
        std::vector<RenderPass *> &render_passes,
        std::function<void(iris::RenderGraph *, const RenderTarget *)> create_render_graph_callback);

    /**
     * Add passes for all the post processing in the supplied passes.
     *
     * @param passes
     *   Passes to added post processing passes for.
     *
     * @param render_passes
     *   Collection of passes to add new passes to.
     */
    void add_post_processing_passes();

    /** Material manager object. */
    MaterialManager &material_manager_;

    /** Mesh manager object. */
    MeshManager &mesh_manager_;

    /** Render target manager object. */
    RenderTargetManager &render_target_manager_;

    /** Collection of created scenes. */
    std::vector<std::unique_ptr<Scene>> scenes_;

    /** Collection of created RenderGraphs. */
    std::vector<std::unique_ptr<RenderGraph>> render_graphs_;

    /** Collection of passes created by the user. */
    std::vector<std::unique_ptr<RenderPass>> user_created_passes_;

    /** Collection of passes created by the engine (in order to render what the user has requested). */
    std::vector<std::unique_ptr<RenderPass>> engine_created_passes_;

    /** Collection of pointers to all render passes in the pipeline. */
    std::vector<RenderPass *> render_passes_;

    /** Collection of created sky box entities. */
    std::unordered_map<const RenderPass *, SingleEntity *> sky_box_entities_;

    /** Collection of created sky box render graphs. */
    std::unordered_map<const RenderPass *, const RenderGraph *> sky_box_render_graphs_;

    /** Flag indicating a scene in the pipeline has changed. */
    bool dirty_;

    /** Width of final render target. */
    std::uint32_t width_;

    /** Height of final render target. */
    std::uint32_t height_;

    /** Cameras created for new passes. */
    std::deque<Camera> cameras_;

    /** Collection of created shadow maps. */
    std::unordered_map<DirectionalLight *, RenderTarget *> shadow_maps_;
};

}
