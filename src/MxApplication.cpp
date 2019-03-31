/*
 * MxApplication.cpp
 *
 *  Created on: Mar 27, 2019
 *      Author: andy
 */

#include <MxApplication.h>
#include <MxWindowlessApplication.h>

#include <iostream>

static MxApplication* app = nullptr;
static MxObject *obj = nullptr;



/**
 * tp_alloc(type) to allocate storage
 * tp_new(type, args) to create blank object
 * tp_init(obj, args) to initialize object
 */

static int MxApplication_init(PyObject *self, PyObject *args, PyObject *kwds)
{
    std::cout << MX_FUNCTION << std::endl;
    return 0;
}



static PyTypeObject ApplicationType = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name = "mechanica.Application",
    .tp_basicsize = sizeof(MxApplication),
    .tp_itemsize = 0,
    .tp_dealloc = 0,
    .tp_print = 0, 
    .tp_getattr = 0, 
    .tp_setattr = 0, 
    .tp_as_async = 0, 
    .tp_repr = 0, 
    .tp_as_number = 0, 
    .tp_as_sequence = 0, 
    .tp_as_mapping = 0, 
    .tp_hash = 0, 
    .tp_call = 0, 
    .tp_str = 0, 
    .tp_getattro = 0, 
    .tp_setattro = 0, 
    .tp_as_buffer = 0, 
    .tp_flags = Py_TPFLAGS_DEFAULT, 
    .tp_doc = 0, 
    .tp_traverse = 0, 
    .tp_clear = 0, 
    .tp_richcompare = 0, 
    .tp_weaklistoffset = 0, 
    .tp_iter = 0, 
    .tp_iternext = 0, 
    .tp_methods = 0, 
    .tp_members = 0, 
    .tp_getset = 0, 
    .tp_base = 0, 
    .tp_dict = 0, 
    .tp_descr_get = 0, 
    .tp_descr_set = 0, 
    .tp_dictoffset = 0, 
    .tp_init = MxApplication_init, 
    .tp_alloc = 0, 
    .tp_new = PyType_GenericNew, 
    .tp_free = 0,  
    .tp_is_gc = 0, 
    .tp_bases = 0, 
    .tp_mro = 0, 
    .tp_cache = 0, 
    .tp_subclasses = 0, 
    .tp_weaklist = 0, 
    .tp_del = 0, 
    .tp_version_tag = 0, 
    .tp_finalize = 0, 
};

/*
static PyTypeObject ApplicationType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "mechanica.Application",
    .tp_doc = "Custom objects",
    .tp_basicsize = sizeof(MxApplication),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = MxApplication_init,
};
*/

PyTypeObject *MxApplication_Type = &ApplicationType;

static PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "custom",
    .m_doc = "Example module that creates an extension type.",
    .m_size = -1,
};

HRESULT MxApplication_init(PyObject* m) {

    std::cout << MX_FUNCTION << std::endl;


    if (PyType_Ready((PyTypeObject *)MxApplication_Type) < 0)
        return E_FAIL;



    Py_INCREF(MxApplication_Type);
    PyModule_AddObject(m, "Application", (PyObject *) MxApplication_Type);

    return 0;
}

HRESULT MxApplication::create(int argc, char** argv, const Configuration& conf)
{
    if(!app) {
        app = new MxWindowlessApplication(argc, argv, conf);

        std::cout << "created new app: " << std::hex << app << std::endl;
    }

    obj = new MxObject();


    return S_OK;
}

HRESULT MxApplication::destroy()
{
    std::cout << "destroying app: " << std::hex << app << std::endl;

    delete obj;
    delete app;
    app = nullptr;
    return S_OK;
}

MxApplication::MxApplication()
{
}

MxApplication* MxApplication::get()
{
    return app;
}