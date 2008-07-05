try:
    from thread import get_ident
except ImportError:
    def get_ident():
        """Stub get_ident() function returning a fake thread id
        for non-threaded environments.
        """
        return 0
