////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace iris
{

class JobSystemManager;
class MaterialManager;
class RenderTargetManager;
class MeshManager;
class PhysicsManager;
class TextureManager;
class WindowManager;
class ResourceManager;

/**
 * This class allows for the runtime registration and retrieval of various manager classes. It is automatically created
 * by the engine and passed to the user via the entry function provided tio start().
 *
 * These managers are factory classes that can create engine components, the reason for all this machinery is:
 *  - it decouples actual implementation from the Context
 *  - start() can register all supported mangers for the current platform and set sane defaults
 *  - allows a user to register their own implementations (e.g. for a different physics library)
 */
class Context
{
  public:
    /**
     * Construct a new Context.
     *
     * @param argc
     *   Program argc.
     *
     * @param argv
     *   Program argv.
     */
    Context(int argc, char **argv);
    ~Context();

    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;
    Context(Context &&);
    Context &operator=(Context &&);

    /**
     * Get the program arguments.
     *
     * @returns
     *   Collection of program arguments.
     */
    const std::vector<std::string> &args() const;

    /**
     * Get the current WindowManager.
     *
     * @returns
     *   Current WindowManager.
     */
    WindowManager &window_manager() const;

    /**
     * Get the current MeshManager.
     *
     * @returns
     *   Current MeshManager.
     */
    MeshManager &mesh_manager() const;

    /**
     * Get the current TextureManager.
     *
     * @returns
     *   Current TextureManager.
     */
    TextureManager &texture_manager() const;

    /**
     * Get the current MaterialManager.
     *
     * @returns
     *   Current MaterialManager.
     */
    MaterialManager &material_manager() const;

    /**
     * Get the current RenderTargetManager.
     *
     * @returns
     *   Current RenderTargetManager.
     */
    RenderTargetManager &render_target_manager() const;

    /**
     * Get the current PhysicsManager.
     *
     * @returns
     *   Current PhysicsManager.
     */
    PhysicsManager &physics_manager() const;

    /**
     * Get the current JobSystemManager.
     *
     * @returns
     *   Current JobSystemManager.
     */
    JobSystemManager &jobs_manager() const;

    /**
     * Get the current ResourceManager.
     *
     * @returns
     *   Current ResourceManager.
     */
    ResourceManager &resource_manager() const;

    /**
     * Register managers for a given api name.
     *
     * @param api
     *   Name of api to register managers to,
     *
     * @param window_manager
     *   New WindowManager.
     *
     * @param mesh_manager
     *   New MeshManager.
     *
     * @param texture_manager
     *   New TextureManager.
     *
     * @param material_manager
     *   New MaterialManager.
     *
     * @param RenderTargetManager
     *   New RenderTargetManager.
     */
    void register_graphics_api(
        const std::string &api,
        std::unique_ptr<WindowManager> window_manager,
        std::unique_ptr<MeshManager> mesh_manager,
        std::unique_ptr<TextureManager> texture_manager,
        std::unique_ptr<MaterialManager> material_manager,
        std::unique_ptr<RenderTargetManager> render_target_manager);

    /**
     * Get the currently set graphics api.
     *
     * @returns
     *   Name of currently set graphics api.
     */
    std::string graphics_api() const;

    /**
     * Set the current graphics api.
     *
     * @param api
     *   New graphics api name.
     */
    void set_graphics_api(const std::string &api);

    /**
     * Get a collection of all registered api names.
     *
     * @returns
     *   Collection of registered api names.
     */
    std::vector<std::string> registered_graphics_apis() const;

    /**
     * Register managers for a given api name.
     *
     * @param api
     *   Name of api to register managers to,
     *
     * @param physics_manager
     *   New PhysicsManager.
     */
    void register_physics_api(const std::string &api, std::unique_ptr<PhysicsManager> physics_manager);

    /**
     * Get the currently set physics api.
     *
     * @returns
     *   Name of currently set physics api.
     */
    std::string physics_api() const;

    /**
     * Set the current physics api.
     *
     * @param api
     *   New physics api name.
     */
    void set_physics_api(const std::string &api);

    /**
     * Get a collection of all registered api names.
     *
     * @returns
     *   Collection of registered api names.
     */
    std::vector<std::string> registered_physics_apis() const;

    /**
     * Register managers for a given api name.
     *
     * @param api
     *   Name of api to register managers to,
     *
     * @param jobs_manager
     *   New JobSystemManager.
     */
    void register_jobs_api(const std::string &api, std::unique_ptr<JobSystemManager> jobs_manager);

    /**
     * Get the currently set jobs api.
     *
     * @returns
     *   Name of currently set jobs api.
     */
    std::string jobs_api() const;

    /**
     * Set the current jobs api.
     *
     * @param api
     *   New jobs api name.
     */
    void set_jobs_api(const std::string &api);

    /**
     * Get a collection of all registered api names.
     *
     * @returns
     *   Collection of registered api names.
     */
    std::vector<std::string> registered_jobs_apis() const;

    /**
     * Set the resource manager object.
     *
     * @param resource_manager
     *   New resource manager.
     */
    void set_resource_manager(std::unique_ptr<ResourceManager> resource_manager);

  private:
    /**
     * Helper struct encapsulating all managers for a graphics api.
     *
     * Note that the member order is important, we want the WindowManager to be destroyed first as some implementations
     * require the Renderer destructor to wait for gpu operations to finish before destroying other resources.
     */
    struct GraphicsApiManagers
    {
        std::unique_ptr<MeshManager> mesh_manager;
        std::unique_ptr<TextureManager> texture_manager;
        std::unique_ptr<MaterialManager> material_manager;
        std::unique_ptr<RenderTargetManager> render_target_manager;
        std::unique_ptr<WindowManager> window_manager;
    };

    /** Map of graphics api name to managers. */
    std::unordered_map<std::string, GraphicsApiManagers> graphics_api_managers_;

    /** Name of current graphics api. */
    std::string graphics_api_;

    /** Map of physics api name to managers. */
    std::unordered_map<std::string, std::unique_ptr<PhysicsManager>> physics_api_managers_;

    /** Name of current physics api. */
    std::string physics_api_;

    /** Map of jobs api name to managers. */
    std::unordered_map<std::string, std::unique_ptr<JobSystemManager>> jobs_api_managers_;

    /** Name of current jobs api. */
    std::string jobs_api_;

    /** Resource manager object. */
    std::unique_ptr<ResourceManager> resource_manager_;

    /** Collection of program arguments. */
    std::vector<std::string> args_;
};

}
