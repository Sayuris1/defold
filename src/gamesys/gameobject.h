#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <ddf/ddf.h>
#include <vectormath/cpp/vectormath_aos.h>

#include "resource.h"
//#include "gameobject_script.h"

using namespace Vectormath::Aos;

namespace dmGameObject
{
    /// Instance handle
    typedef struct Instance* HInstance;

    // Script handle
    typedef struct Script* HScript;
    /// Instance handle
    typedef struct ScriptInstance* HScriptInstance;
    /// Script context
    typedef struct ScriptContext* HScriptContext;

    /// Collection handle
    typedef struct Collection* HCollection;

    /**
     * Result enum
     */
    enum Result
    {
        RESULT_OK = 0,                      //!< RESULT_OK
        RESULT_OUT_OF_RESOURCES = -1,       //!< RESULT_OUT_OF_RESOURCES
        RESULT_ALREADY_REGISTERED = -2,     //!< RESULT_ALREADY_REGISTERED
        RESULT_IDENTIFIER_IN_USE = -3,      //!< RESULT_IDENTIFIER_IN_USE
        RESULT_IDENTIFIER_ALREADY_SET = -4, //!< RESULT_IDENTIFIER_ALREADY_SET
        RESULT_COMPONENT_NOT_FOUND = -5,    //!< RESULT_COMPONENT_NOT_FOUND
        RESULT_MAXIMUM_HIEARCHICAL_DEPTH = -6, //!< RESULT_MAXIMUM_HIEARCHICAL_DEPTH
        RESULT_INVALID_OPERATION = -7,      //!< RESULT_INVALID_OPERATION
        RESULT_UNKNOWN_ERROR = -1000,       //!< RESULT_UNKNOWN_ERROR
    };

    /**
     * Create result enum
     */
    enum CreateResult
    {
        CREATE_RESULT_OK = 0,              //!< CREATE_RESULT_OK
        CREATE_RESULT_UNKNOWN_ERROR = -1000,//!< CREATE_RESULT_UNKNOWN_ERROR
    };

    /**
     * Create result enum
     */
    enum UpdateResult
    {
        UPDATE_RESULT_OK = 0,              //!< UPDATE_RESULT_OK
        UPDATE_RESULT_UNKNOWN_ERROR = -1000,//!< UPDATE_RESULT_UNKNOWN_ERROR
    };

    /**
     * Update context
     */
    struct UpdateContext
    {
        /// DDF descriptor for global data
        dmDDF::Descriptor* m_DDFGlobalDataDescriptor;
        /// Global read-only data
        void*              m_GlobalData;
        /// Time step
        float              m_DT;
    };

    extern const uint32_t UNNAMED_IDENTIFIER;

    #define DMGAMEOBJECT_EVENT_NAME "script_event"
    #define DMGAMEOBJECT_EVENT_SOCKET_NAME "script"
    #define DMGAMEOBJECT_REPLY_EVENT_SOCKET_NAME "script_reply"

    /**
     * Game script event data
     */
    struct ScriptEventData
    {
        /// Subject
        HInstance m_Instance;

        /// Subject instance component index. Set to 0xff to broadcast to all components. Set to 0 to target scripts.
        uint8_t   m_Component;
        uint8_t   m_Pad[3];

        /// Eventhash
        uint32_t  m_EventHash;

        /// Pay-load DDF descriptor. NULL if not present
        const dmDDF::Descriptor* m_DDFDescriptor;
        /// Pay-load (DDF). Optional. Requires non-NULL m_DDFDescriptor
        uint8_t                  m_DDFData[0];
    };

    /**
     * Component create function
     * @param collection Collection handle
     * @param instance Game object instance
     * @param resource Component resource
     * @param context User context
     * @param user_data User data storage pointer
     * @return CREATE_RESULT_OK on success
     */
    typedef CreateResult (*ComponentCreate)(HCollection collection,
                                            HInstance instance,
                                            void* resource,
                                            void* context,
                                            uintptr_t* user_data);

    /**
     * Component init function
     * @param collection Collection handle
     * @param instance Game object instance
     * @param resource Component resource
     * @param context User context
     * @param user_data User data storage pointer
     * @return CREATE_RESULT_OK on success
     */
    typedef CreateResult (*ComponentInit)(HCollection collection,
                                            HInstance instance,
                                            void* context,
                                            uintptr_t* user_data);

    /**
     * Component destroy function
     * @param collection Collection handle
     * @param instance Game object instance
     * @param context User context
     * @param user_data User data storage pointer
     * @return CREATE_RESULT_OK on success
     */
    typedef CreateResult (*ComponentDestroy)(HCollection collection,
                                             HInstance instance,
                                             void* context,
                                             uintptr_t* user_data);

    /**
     * Component update function. Updates all component of this type for all game objects
     * @param collection Collection handle
     * @param update_context Update context
     * @param context User context
     */
    typedef UpdateResult (*ComponentsUpdate)(HCollection collection,
                                     const UpdateContext* update_context,
                                     void* context);

    /**
     * Component on-event function. Called when event is sent to this component
     * @param collection Collection handle
     * @param instance Instance handle
     * @param context User context
     * @param user_data User data storage pointer
     */
    typedef UpdateResult (*ComponentOnEvent)(HCollection collection,
                                     HInstance instance,
                                     const ScriptEventData* event_data,
                                     void* context,
                                     uintptr_t* user_data);

    /**
     * Collection of component registration data.
     */
    struct ComponentType
    {
        ComponentType();

        uint32_t            m_ResourceType;
        const char*         m_Name;
        void*               m_Context;
        ComponentCreate     m_CreateFunction;
        ComponentInit       m_InitFunction;
        ComponentDestroy    m_DestroyFunction;
        ComponentsUpdate    m_UpdateFunction;
        ComponentOnEvent    m_OnEventFunction;
        uint32_t            m_InstanceHasUserData : 1;
    };

    /**
     * Initialize system
     */
    void Initialize();

    /**
     * Finalize system
     */
    void Finalize();

    /**
     * Register DDF type
     * @param descriptor Descriptor
     * @return RESULT_OK on success
     */
    Result RegisterDDFType(const dmDDF::Descriptor* descriptor);

    /**
     * Creates a new gameobject collection
     * @param factory Resource factory. Must be valid during the life-time of the collection
     * @param max_instances Max instances in this collection
     * @return HCollection
     */
    HCollection NewCollection(dmResource::HFactory factory, uint32_t max_instances);

    /**
     * Deletes a gameobject collection
     * @param collection
     */
    void DeleteCollection(HCollection collection);

    /**
     * Register a new component type
     * @param collection Gameobject collection
     * @param type Collection of component type registration data
     * @return RESULT_OK on success
     */
    Result RegisterComponentType(HCollection collection, const ComponentType& type);

    /**
     * Create a new gameobject instane
     * @param collection Gameobject collection
     * @param prototype_name Prototype file name
     * @param update_context Update context
     * @return New gameobject instance. NULL if any error occured
     */
    HInstance New(HCollection collection, const char* prototype_name);

    /**
     * Delete gameobject instance
     * @param collection Gameobject collection
     * @param instance Gameobject instance
     */
    void Delete(HCollection collection, HInstance instance);

    /**
     * Delete all gameobject instances in the collection
     * @param collection Gameobject collection
     */
    void DeleteAll(HCollection collection);

    /**
     * Set instance identifier. Must be unique within the collection.
     * @param collection Collection
     * @param instance Instance
     * @param identifier Identifier
     * @return RESULT_OK on success
     */
    Result SetIdentifier(HCollection collection, HInstance instance, const char* identifier);

    /**
     * Get instance identifier
     * @param instance Instance
     * @return Identifer. dmGameObject::UNNAMED_IDENTIFIER if not set.
     */
    uint32_t GetIdentifier(HInstance instance);

    /**
     * Get instance from identifier
     * @param collection Collection
     * @param identifier Identifier
     * @return Instance. NULL if instance isn't found.
     */
    HInstance GetInstanceFromIdentifier(HCollection collection, uint32_t identifier);

    /**
     * Post named event to component instance
     * @param instance Instance
     * @param component_name Component name. NULL if for sending to script
     * @param event Named event
     * @return RESULT_OK on success
     */
    Result PostNamedEvent(HInstance instance, const char* component_name, const char* event, const dmDDF::Descriptor* ddf_desc = 0x0, char* ddf_data = 0x0);

    /**
     * Set integer property in instance script
     * @param instance Instnce
     * @param key Key
     * @param value Value
     */
    void SetScriptIntProperty(HInstance instance, const char* key, int32_t value);

    /**
     * Set float property in instance script
     * @param instance Instnce
     * @param key Key
     * @param value Value
     */
    void SetScriptFloatProperty(HInstance instance, const char* key, float value);

    /**
     * Set string property in instance script
     * @param instance Instnce
     * @param key Key
     * @param value Value
     */
    void SetScriptStringProperty(HInstance instance, const char* key, const char* value);

    /**
     * Initializes a game object instance in the supplied collection.
     * @param collection Game object collection
     * @param update_context Update context
     */
    bool Init(HCollection collection, HInstance instance, const UpdateContext* update_context);

    /**
     * Initializes all game object instances in the supplied collection.
     * @param collection Game object collection
     * @param update_context Update context
     */
    bool Init(HCollection collection, const UpdateContext* update_context);

    /**
     * Update all gameobjects and its components and dispatches all event to script
     * @param collection Gameobject collection
     * @param update_context Update context
     * @return True on success
     */
    bool Update(HCollection collection, const UpdateContext* update_context);

    /**
     * Retrieve a factory from the specified collection
     * @param collection Game object collection
     * @return The resource factory bound to the specified collection
     */
    dmResource::HFactory GetFactory(HCollection collection);

    /**
     * Set gameobject instance position
     * @param instance Gameobject instance
     * @param position New Position
     */
    void SetPosition(HInstance instance, Point3 position);

    /**
     * Get gameobject instance position
     * @param instance Gameobject instance
     * @return Position
     */
    Point3 GetPosition(HInstance instance);

    /**
     * Set gameobject instance rotation
     * @param instance Gameobject instance
     * @param position New Position
     */
    void SetRotation(HInstance instance, Quat rotation);

    /**
     * Get gameobject instance rotation
     * @param instance Gameobject instance
     * @return Position
     */
    Quat GetRotation(HInstance instance);

    /**
     * Get gameobject instance world position
     * @param instance Gameobject instance
     * @return World position
     */
    Point3 GetWorldPosition(HInstance instance);

    /**
     * Get gameobject instance world rotation
     * @param instance Gameobject instance
     * @return World rotation
     */
    Quat GetWorldRotation(HInstance instance);

    /**
     * Set parent instance to child
     * @note Instances must belong to the same collection
     * @param child Child instance
     * @param parent Parent instance
     * @return RESULT_OK on success. RESULT_MAXIMUM_HIEARCHICAL_DEPTH if parent at maximal level
     */
    Result SetParent(HInstance child, HInstance parent);

    /**
     * Get parent instance if exists
     * @param instance Gameobject instance
     * @return Parent instance. NULL if passed instance is rooted
     */
    HInstance GetParent(HInstance instance);

    /**
     * Get instance hierarchical depth
     * @param instance Gameobject instance
     * @return Hierarchical depth
     */
    uint32_t GetDepth(HInstance instance);

    /**
     * Get child count
     * @note O(n) operation. Should only be used for debugging purposes.
     * @param instance Gameobject instance
     * @return Child count
     */
    uint32_t GetChildCount(HInstance instance);

    /**
     * Test if "child" is a direct parent of "parent"
     * @param child Child Gamebject
     * @param parent Parent Gameobject
     * @return True if child of
     */
    bool IsChildOf(HInstance child, HInstance parent);

    /**
     * Register all resource types in resource factory
     * @param factory Resource factory
     * @return dmResource::FactoryResult
     */
    dmResource::FactoryResult RegisterResourceTypes(dmResource::HFactory factory);

    /**
     * Register all component types in collection
     * @param factory Resource factory
     * @param collection Collection
     * @param update_context An update context that will be used in the script components so it must live for as long as the collection.
     * @return Result
     */
    Result RegisterComponentTypes(dmResource::HFactory factory, HCollection collection, HScriptContext script_context);

    /**
     * Creates a script context
     * @param update_context Update context to bind to the script context
     * @return The script context
     */
    HScriptContext CreateScriptContext(UpdateContext* update_context);

    /**
     * Destroy a script context
     * @param script_context Script context to destroy
     */
    void DestroyScriptContext(HScriptContext script_context);
}

#endif // GAMEOBJECT_H
