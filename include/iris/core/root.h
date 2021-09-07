#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace iris
{

class MeshManager;
class PhysicsManager;
class TextureManager;
class WindowManager;

/**
 * This class allows for the runtime registration and retrieval of various
 * manager classes. It is a singleton.
 */
class Root
{
  public:
    ~Root() = default;

    /**
     * Get the current WindowManager.
     *
     * @returns
     *   Current WindowManager.
     */
    static WindowManager &window_manager();

    /**
     * Get the current MeshManager.
     *
     * @returns
     *   Current MeshManager.
     */
    static MeshManager &mesh_manager();

    /**
     * Get the current TextureManager.
     *
     * @returns
     *   Current TextureManager.
     */
    static TextureManager &texture_manager();

    /**
     * Get the current PhysicsManager.
     *
     * @returns
     *   Current PhysicsManager.
     */
    static PhysicsManager &physics_manager();

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
     */
    static void register_graphics_api(
        const std::string &api,
        std::unique_ptr<WindowManager> window_manager,
        std::unique_ptr<MeshManager> mesh_manager,
        std::unique_ptr<TextureManager> texture_manager);

    /**
     * Get the currently set graphics api.
     *
     * @returns
     *   Name of currently set graphics api.
     */
    static std::string graphics_api();

    /**
     * Set the current graphics api.
     *
     * @param api
     *   New graphics api name.
     */
    static void set_graphics_api(const std::string &api);

    /**
     * Get a collection of all registered api names.
     *
     * @returns
     *   Collection of registered api names.
     */
    static std::vector<std::string> registered_graphics_apis();

    /**
     * Register managers for a given api name.
     *
     * @param api
     *   Name of api to register managers to,
     *
     * @param physics_manager
     *   New PhysicsManager.
     */
    static void register_physics_api(
        const std::string &api,
        std::unique_ptr<PhysicsManager> physics_manager);

    /**
     * Get the currently set physics api.
     *
     * @returns
     *   Name of currently set physics api.
     */
    static std::string physics_api();

    /**
     * Set the current physics api.
     *
     * @param api
     *   New physics api name.
     */
    static void set_physics_api(const std::string &api);

    /**
     * Get a collection of all registered api names.
     *
     * @returns
     *   Collection of registered api names.
     */
    static std::vector<std::string> registered_physics_apis();

  private:
    // private to force access through above public static methods
    Root();
    static Root &instance();

    // these are the member function equivalents of the above static methods
    // see their docs for details

    WindowManager &window_manager_impl() const;

    MeshManager &mesh_manager_impl() const;

    TextureManager &texture_manager_impl() const;

    PhysicsManager &physics_manager_impl() const;

    void register_graphics_api_impl(
        const std::string &api,
        std::unique_ptr<WindowManager> window_manager,
        std::unique_ptr<MeshManager> mesh_manager,
        std::unique_ptr<TextureManager> texture_manager);

    std::string graphics_api_impl() const;

    void set_graphics_api_impl(const std::string &api);

    std::vector<std::string> registered_graphics_apis_impl() const;

    void register_physics_api_impl(
        const std::string &api,
        std::unique_ptr<PhysicsManager> physics_manager);

    std::string physics_api_impl() const;

    void set_physics_api_impl(const std::string &api);

    std::vector<std::string> registered_physics_apis_impl() const;

    /**
     * Helper struct encapsulating all managers for a graphics api.
     */
    struct GraphicsApiManagers
    {
        std::unique_ptr<WindowManager> window_manager;
        std::unique_ptr<MeshManager> mesh_manager;
        std::unique_ptr<TextureManager> texture_manager;
    };

    /** Map of graphics api name to managers. */
    std::unordered_map<std::string, GraphicsApiManagers> graphics_api_managers_;

    /** Name of current graphics api. */
    std::string graphics_api_;

    /** Map of physics api name to managers. */
    std::unordered_map<std::string, std::unique_ptr<PhysicsManager>>
        physics_api_managers_;

    /** Name of current physics api. */
    std::string physics_api_;
};

}
