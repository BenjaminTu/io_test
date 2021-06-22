import input_stream as ist
import io

class InputStream:
    def __init__(self, stream):
        self.stream = stream
        self.opt = ist.aws_crt_input_stream_options_new()
        ist.aws_crt_input_stream_options_set_seek(self.opt, self.seek)
        ist.aws_crt_input_stream_options_set_read(self.opt, self.read)
        ist.aws_crt_input_stream_options_set_get_status(self.opt, self.get_status)
        ist.aws_crt_input_stream_options_set_get_length(self.opt, self.get_length)
        ist.aws_crt_input_stream_options_set_destroy(self.opt, self.destroy)
        self.i_stream = ist.aws_crt_input_stream_new(self.opt)

    def seek(self, offset, whence):
        return self.stream.seek(offset, whence)
        print("calling seek")

    def read(self, m):
        print("calling read")
        data = self.stream.read(len(m))
        n = len(data)
        m[:n] = data
        return n

    def get_status(self):
        print("calling get_status")

    def get_length(self):
        print("calling get_length")

    def destroy(self):
        print("calling destroy")

class InputStreamTest():
    def _test(self, python_stream, expected):
        input_stream = InputStream(python_stream)
        result = bytearray()
        fixed_mv_len = 4
        fixed_mv = memoryview(bytearray(fixed_mv_len))
        while True:
            read_len = input_stream.read(fixed_mv)
            if read_len is None:
                continue
            if read_len == 0:
                break
            if read_len > 0:
                assert read_len <= fixed_mv_len
                result += fixed_mv[0:read_len]

        assert expected == result

    def test_read_official_io(self):
        # Read from a class defined in the io module
        src_data = b'a long string here'
        python_stream = io.BytesIO(src_data)
        self._test(python_stream, src_data)

    def test_read_duck_typed_io(self):
        # Read from a class defined in the io module
        src_data = b'a man a can a planal canada'
        python_stream = MockPythonStream(src_data)
        self._test(python_stream, src_data)


# test = InputStreamTest()
# test.test_read_official_io()
# test.test_read_duck_typed_io()
src_data = b'a long string here'
python_stream = io.BytesIO(src_data)
test = InputStream(python_stream)
print(test.i_stream)
ist.test_io(test.i_stream)

# print(aws.aws_crt_init())
# print(aws.aws_crt_clean_up())
# print(aws.aws_crt_test_error(1))
# print(aws.aws_crt_mem_acquire(2))
# print(aws.aws_crt_mem_release(0x01234567))
# print(aws.aws_crt_default_allocator())