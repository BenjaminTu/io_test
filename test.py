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
        print("calling seek from python")
        self.stream.seek(offset, whence)

    def read(self, m):
        print("calling read from python")
        data = self.stream.read(len(m))
        n = len(data)
        m[:n] = data
        return n

    def get_status(self):
        print("calling get_status from python")

    def get_length(self):
        print("calling get_length from python")

    def destroy(self):
        print("calling destroy from python")

class MockPythonStream:
    """For testing duck-typed stream classes.
    Doesn't inherit from io.IOBase. Doesn't implement readinto()"""

    def __init__(self, src_data):
        self.data = bytes(src_data)
        self.len = len(src_data)
        self.pos = 0

    def seek(self, where):
        self.pos = where

    def tell(self):
        return self.pos

    def read(self, amount=None):
        if amount is None:
            amount = self.len - self.pos
        else:
            amount = min(amount, self.len - self.pos)
        prev_pos = self.pos
        self.pos += amount
        return self.data[prev_pos: self.pos]

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



# read/seek test from python
# test = InputStreamTest()
# test.test_read_official_io()
# test.test_read_duck_typed_io()


# callback function test
src_data = b'a long string here'
python_stream = io.BytesIO(src_data)
test = InputStream(python_stream)
ist.test_io(test.i_stream)