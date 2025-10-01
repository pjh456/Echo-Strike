#ifndef INCLUDE_CLASS_MARCOS
#define INCLUDE_CLASS_MARCOS

#define CLASS_READONLY_PROPERTY(type, name) \
private:                                    \
    type m_##name;                          \
                                            \
public:                                     \
    type get_##name() const noexcept { return m_##name; }

#define CLASS_PROPERTY(type, name)      \
    CLASS_READONLY_PROPERTY(type, name) \
    void set_##name(const type &p_##name) noexcept { m_##name = p_##name; }

#define CLASS_POINTER(type, name)                          \
private:                                                   \
    type *m_##name;                                        \
                                                           \
public:                                                    \
    type *get_##name() const noexcept { return m_##name; } \
    void set_##name(type *p_##name)                        \
    {                                                      \
        if (m_##name != nullptr)                           \
            delete m_##name;                               \
        m_##name = p_##name;                               \
    }

#define CLASS_REFERENCE(type, name)                  \
private:                                             \
    type &m_##name;                                  \
                                                     \
public:                                              \
    type &get_##name() noexcept { return m_##name; } \
    const type &get_##name() const noexcept { return m_##name; }

#define COPY_CONSTRUCTOR(classname, scope) classname(const classname &) scope
#define MOVE_CONSTRUCTOR(classname, scope) classname(classname &&) noexcept scope
#define DESTRUCTOR(classname, scope) ~classname() scope
#define VIRTUAL_DESTRUCTOR(classname, scope) virtual ~classname() scope

#define DEFAULT_COPY_CONSTRUCTOR(classname) COPY_CONSTRUCTOR(classname, = default;)
#define DEFAULT_MOVE_CONSTRUCTOR(classname) MOVE_CONSTRUCTOR(classname, = default;)
#define DEFAULT_DESTRUCTOR(classname) DESTRUCTOR(classname, = default;)
#define DEFAULT_VIRTUAL_DESTRUCTOR(classname) VIRTUAL_DESTRUCTOR(classname, = default;)
#define DEFAULT_CONSTRUCTOR(classname)  \
    DEFAULT_COPY_CONSTRUCTOR(classname) \
    DEFAULT_MOVE_CONSTRUCTOR(classname) \
    DEFAULT_DESTRUCTOR(classname)
#define DEFAULT_VIRTUAL_CONSTRUCTOR(classname) \
    DEFAULT_COPY_CONSTRUCTOR(classname)        \
    DEFAULT_MOVE_CONSTRUCTOR(classname)        \
    DEFAULT_VIRTUAL_DESTRUCTOR(classname)

#define COPY_EQUAL_OPERATOR(classname, scope) classname &operator=(const classname &other) scope
#define MOVE_EQUAL_OPERATOR(classname, scope) classname &operator=(classname &&other) noexcept scope

#define DEFAULT_COPY_EQUAL_OPERATOR(classname) COPY_EQUAL_OPERATOR(classname, = default;)
#define DEFAULT_MOVE_EQUAL_OPERATOR(classname) MOVE_EQUAL_OPERATOR(classname, = default;)
#define DEFAULT_EQUAL_OPERATOR(classname)  \
    DEFAULT_COPY_EQUAL_OPERATOR(classname) \
    DEFAULT_MOVE_EQUAL_OPERATOR(classname)

#endif // INCLUDE_CLASS_MARCOS