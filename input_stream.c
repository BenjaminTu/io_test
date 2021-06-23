#include <Python.h>
#include "api.h"

typedef struct py_stream_vtable {
    /* Callback functions */
    PyObject *seek;
    PyObject *read;
    PyObject *get_status;
    PyObject *get_length;
    PyObject *destroy;
} py_stream_vtable;

typedef struct aws_crt_input_stream_impl {
    py_stream_vtable *vtable;
    _Bool is_end_of_stream;
} aws_crt_input_stream_impl;

static int s_py_stream_seek(void *user_data, int64_t offset, aws_crt_input_stream_seek_basis basis) {
    aws_crt_input_stream_impl *impl = user_data;
    py_stream_vtable *vtable = impl->vtable;
    PyObject *args = Py_BuildValue("(Li)", offset, basis);
    PyObject_Call(vtable->seek, args, NULL);
    impl->is_end_of_stream = 0;
    return 1;
}

static int s_py_stream_read(void *user_data, uint8_t *dest, size_t dest_length) {
    aws_crt_input_stream_impl *impl = user_data;
    py_stream_vtable *vtable = impl->vtable;

    PyObject *mem_view = PyMemoryView_FromMemory((char *)dest, dest_length, PyBUF_WRITE);

    PyObject *args = Py_BuildValue("(O)", mem_view);
    PyObject *ret = PyObject_Call(vtable->read, args, NULL);
    
    Py_ssize_t bytes_read = 0;
    if (ret != Py_None) {
        bytes_read = PyLong_AsSsize_t(ret);
        if (bytes_read == -1 && PyErr_Occurred()) {
            return 0;
        }

        if (bytes_read < 0) {
            return 0;
        }

        if (bytes_read == 0) {
            impl->is_end_of_stream = 1;
        }
    }

    Py_XDECREF(mem_view);

    return 1;
}

static int s_py_stream_get_length(void *user_data, int64_t *out_length) {
    (void)out_length;
    aws_crt_input_stream_impl *impl = user_data;
    py_stream_vtable *vtable = impl->vtable;
    PyObject *args = Py_BuildValue("()");
    PyObject_Call(vtable->get_length, args, NULL);
    // NOT IMPLEMENTED
    return 1;
}

static int s_py_stream_get_status(void *user_data, aws_crt_input_stream_status *out_status) {
    (void)out_status;
    aws_crt_input_stream_impl *impl = user_data;
    py_stream_vtable *vtable = impl->vtable;
    PyObject *args = Py_BuildValue("()");
    PyObject_Call(vtable->get_status, args, NULL);
    out_status->is_valid = 1;
    out_status->is_end_of_stream = impl->is_end_of_stream;
    return 1;
}

static void s_py_stream_destroy(void *user_data) {
    aws_crt_input_stream_impl *impl = user_data;
    py_stream_vtable *vtable = impl->vtable;
    PyObject *args = Py_BuildValue("()");

    PyObject_Call(vtable->destroy, args, NULL);
    Py_XDECREF(vtable->seek);
    Py_XDECREF(vtable->read);
    Py_XDECREF(vtable->get_length);
    Py_XDECREF(vtable->get_status);
    Py_XDECREF(vtable->destroy);
}

static PyObject *method_aws_crt_input_stream_options_new(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    aws_crt_input_stream_impl *impl = (aws_crt_input_stream_impl *) aws_crt_mem_calloc(1, sizeof(aws_crt_input_stream_impl));
    py_stream_vtable* vtable = (py_stream_vtable *) aws_crt_mem_calloc(1, sizeof(py_stream_vtable));
    impl->vtable = vtable;
    PyObject *ret = PyCapsule_New(impl, "aws_crt_input_stream_impl *", NULL);
    return ret;
}

static PyObject *method_aws_crt_input_stream_options_set_seek(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    // bind callback function to s_py_stream_seek
    PyObject *a; // impl
    PyObject *b; // seek_fn

    /* Parse arguments */
    if (!PyArg_ParseTuple(args, "OO", &a, &b)) {
        return NULL;
    }

    if (!PyCallable_Check(b)) {
        PyErr_SetString(PyExc_TypeError, "a callable is required");
        return 0;
    }

    aws_crt_input_stream_impl * c;
    c = (aws_crt_input_stream_impl *) PyCapsule_GetPointer(a, "aws_crt_input_stream_impl *");
    c->vtable->seek = b;
    Py_INCREF(b);

    Py_RETURN_NONE;
}

static PyObject *method_aws_crt_input_stream_options_set_read(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    // bind callback function to s_py_stream_read
    PyObject *a; // impl
    PyObject *b; // read_fn

    /* Parse arguments */
    if (!PyArg_ParseTuple(args, "OO", &a, &b)) {
        return NULL;
    }

    if (!PyCallable_Check(b)) {
        PyErr_SetString(PyExc_TypeError, "a callable is required");
        return 0;
    }

    aws_crt_input_stream_impl * c;
    c = (aws_crt_input_stream_impl *) PyCapsule_GetPointer(a, "aws_crt_input_stream_impl *");
    c->vtable->read = b;
    Py_INCREF(b);

    Py_RETURN_NONE;
}

static PyObject *method_aws_crt_input_stream_options_set_get_status(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    // bind callback function to s_py_stream_get_status
    PyObject *a; // impl
    PyObject *b; // get_status_fn

    /* Parse arguments */
    if (!PyArg_ParseTuple(args, "OO", &a, &b)) {
        return NULL;
    }

    if (!PyCallable_Check(b)) {
        PyErr_SetString(PyExc_TypeError, "a callable is required");
        return 0;
    }

    aws_crt_input_stream_impl * c;
    c = (aws_crt_input_stream_impl *) PyCapsule_GetPointer(a, "aws_crt_input_stream_impl *");
    c->vtable->get_status = b;
    Py_INCREF(b);

    Py_RETURN_NONE;
}

static PyObject *method_aws_crt_input_stream_options_set_get_length(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    // bind callback function to s_py_stream_get_length
    PyObject *a; // impl
    PyObject *b; // get_length_fn

    /* Parse arguments */
    if (!PyArg_ParseTuple(args, "OO", &a, &b)) {
        return NULL;
    }

    if (!PyCallable_Check(b)) {
        PyErr_SetString(PyExc_TypeError, "a callable is required");
        return 0;
    }

    aws_crt_input_stream_impl * c;
    c = (aws_crt_input_stream_impl *) PyCapsule_GetPointer(a, "aws_crt_input_stream_impl *");
    c->vtable->get_length = b;
    Py_INCREF(b);

    Py_RETURN_NONE;
}

static PyObject *method_aws_crt_input_stream_options_set_destroy(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    // bind callback function to s_py_stream_destroy
    PyObject *a; // impl
    PyObject *b; // destroy_fn

    /* Parse arguments */
    if (!PyArg_ParseTuple(args, "OO", &a, &b)) {
        return NULL;
    }

    if (!PyCallable_Check(b)) {
        PyErr_SetString(PyExc_TypeError, "a callable is required");
        return 0;
    }

    aws_crt_input_stream_impl * c;
    c = (aws_crt_input_stream_impl *) PyCapsule_GetPointer(a, "aws_crt_input_stream_impl *");
    c->vtable->destroy = b;
    Py_INCREF(b);

    Py_RETURN_NONE;
}

static PyObject *method_aws_crt_input_stream_new(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    PyObject *a;

    /* Parse arguments */
    if (!PyArg_ParseTuple(args, "O", &a)) {
        return NULL;
    }
    aws_crt_input_stream_impl *b;
    b = (aws_crt_input_stream_impl *) PyCapsule_GetPointer(a, "aws_crt_input_stream_impl *");

    aws_crt_input_stream_options *c;
    c = aws_crt_input_stream_options_new();
    aws_crt_input_stream_options_set_user_data(c, (void *)b);
    aws_crt_input_stream_options_set_seek(c, s_py_stream_seek);
    aws_crt_input_stream_options_set_read(c, s_py_stream_read);
    aws_crt_input_stream_options_set_get_status(c, s_py_stream_get_status);
    aws_crt_input_stream_options_set_get_length(c, s_py_stream_get_length);
    aws_crt_input_stream_options_set_destroy(c, s_py_stream_destroy);
    PyObject *ret = PyCapsule_New((void *)aws_crt_input_stream_new(c), "aws_crt_input_stream *", NULL);
    aws_crt_input_stream_options_release(c);
    return (ret);
}

static PyObject *method_aws_crt_input_stream_release(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    PyObject *a;

    /* Parse arguments */
    if (!PyArg_ParseTuple(args, "O", &a)) {
        return NULL;
    }
    aws_crt_input_stream *b;
    b = (aws_crt_input_stream *) PyCapsule_GetPointer(a, "aws_crt_input_stream *");
    aws_crt_input_stream_release(b);
    Py_RETURN_NONE;
}

static PyObject *method_aws_crt_input_stream_seek(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    Py_RETURN_NONE;
}

static PyObject *method_aws_crt_input_stream_read(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    Py_RETURN_NONE;
}

static PyObject *method_aws_crt_input_stream_get_status(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    Py_RETURN_NONE;
}

static PyObject *method_aws_crt_input_stream_get_length(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    Py_RETURN_NONE;
}

static PyObject *method_test_io(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    PyObject *a;

    /* Parse arguments */
    if (!PyArg_ParseTuple(args, "O", &a)) {
        return NULL;
    }
    aws_crt_input_stream *b;
    b = (aws_crt_input_stream *) PyCapsule_GetPointer(a, "aws_crt_input_stream *");
    aws_crt_input_stream_status *c = aws_crt_mem_calloc(1, sizeof(aws_crt_input_stream_status));
    aws_crt_input_stream_get_status(b, c);
    printf("%d\n", c->is_end_of_stream);
    printf("%d\n", c->is_valid);

    uint8_t *d;
    d = aws_crt_mem_calloc(1, 4 * sizeof(char));
    aws_crt_input_stream_read(b, d, 4);
    printf("buffer from c: %s\n", d);
    aws_crt_input_stream_read(b, d, 4);
    printf("buffer from c: %s\n", d);
    aws_crt_input_stream_read(b, d, 4);
    printf("buffer from c: %s\n", d);
    aws_crt_input_stream_read(b, d, 4);
    printf("buffer from c: %s\n", d);
    aws_crt_input_stream_read(b, d, 4);
    printf("buffer from c: %s\n", d);
    aws_crt_input_stream_read(b, d, 4);
    printf("buffer from c: %s\n", d);
    
    Py_RETURN_NONE;
}

static PyMethodDef input_stream_methods[] = {
    {"aws_crt_input_stream_options_new", method_aws_crt_input_stream_options_new, METH_VARARGS, ""},
    {"aws_crt_input_stream_options_set_seek", method_aws_crt_input_stream_options_set_seek, METH_VARARGS, ""},
    {"aws_crt_input_stream_options_set_read", method_aws_crt_input_stream_options_set_read, METH_VARARGS, ""},
    {"aws_crt_input_stream_options_set_get_status", method_aws_crt_input_stream_options_set_get_status, METH_VARARGS, ""},
    {"aws_crt_input_stream_options_set_get_length", method_aws_crt_input_stream_options_set_get_length, METH_VARARGS, ""},
    {"aws_crt_input_stream_options_set_destroy", method_aws_crt_input_stream_options_set_destroy, METH_VARARGS, ""},
    {"aws_crt_input_stream_new", method_aws_crt_input_stream_new, METH_VARARGS, ""},
    {"aws_crt_input_stream_release", method_aws_crt_input_stream_release, METH_VARARGS, ""},
    {"aws_crt_input_stream_seek", method_aws_crt_input_stream_seek, METH_VARARGS, ""},
    {"aws_crt_input_stream_read", method_aws_crt_input_stream_read, METH_VARARGS, ""},
    {"aws_crt_input_stream_get_status", method_aws_crt_input_stream_get_status, METH_VARARGS, ""},
    {"aws_crt_input_stream_get_length", method_aws_crt_input_stream_get_length, METH_VARARGS, ""},
    {"test_io", method_test_io, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef input_stream_module = {
    PyModuleDef_HEAD_INIT,
    "input_stream",
    "",
    -1,
    input_stream_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC PyInit_input_stream(void) {
    return PyModule_Create(&input_stream_module);
}