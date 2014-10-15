import collections

class FilterClass(type):
    @classmethod
    def __prepare__(metacls, name, bases, **kwds):
        return collections.OrderedDict()

    def __new__(cls, name, bases, namespace, **kwds):
        result = type.__new__(cls, name, bases, dict(namespace))
        result._filters = [value for value in namespace.values() if hasattr(value, '_filter')]
        return result

def stringfilter(func):
    func._filter = True
    return func

# class StringFilter:

#     def __init__(self, func):
#         self._func = func
#         self._filter = True

#     def __call__(self, obj, string):
#         return self._func(obj, string)

class StringProcessor(metaclass=FilterClass):
    def __call__(self, string):
        _string = string
        for _filter in self._filters:
            _string = _filter(self, _string)

        return _string


class MyStringProcessor(StringProcessor):
    @stringfilter
    def capitalize(self, string):
        return string.capitalize()

    @stringfilter
    def remove_double_spaces(self, string):
        return string.replace('  ', ' ')

if __name__ == '__main__':
    msp = MyStringProcessor()
    msp._filters
    input_string = "a test  string"
    output_string = msp(input_string)
    print ("INPUT STRING:", input_string)
    print ("OUTPUT STRING:", output_string)

