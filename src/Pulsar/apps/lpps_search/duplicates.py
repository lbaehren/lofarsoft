'''
Reject duplicate pulsar detections across several values of Z.
'''
from __future__ import division

ERR = 0.01


def candidate_matcher(l, i, ii):
    # Assumptions: candidate.p > 0 and candidates.dm >= 0 (always!)
    c1 = l[i]
    c2 = l[ii]

    if 2 * abs(c1.p - c2.p) / (c1.p + c2.p) > ERR:
        return False

    try:
        tmp = 2 * abs(c1.DM - c2.DM) / (c1.DM + c2.DM)
    except ZeroDivisionError, e:
        return True # both are at DM = 0
    else:
        return tmp < ERR 

class SymmetricalUnweightedGraph(object):
    def __init__(self):
        self.dict = {}

    def add(self, i):
        self.dict[i] = set([])

    def connect(self, i, ii):
        self.dict[i].add(ii)
        self.dict[ii].add(i)

    def disconnect(self, i, ii):
        self.dict[i].pop(ii)
        self.dict[ii].pop(i)

    def remove(self, i):
        references = self.dict[i]
        for idx in references:
            self.dict[idx].remove(i)
        del self.dict[i] 

    def connections(self, i):
        return iter(self.dict[i])

    def connected(self, i):
        l_open = set(self.connections(i))
        l_closed = set([i])
        
        while l_open:
            idx = l_open.pop()
            l_closed.add(idx)
            
            tmp = set(self.connections(i))
            tmp -= l_closed
            
            l_open |= tmp

        return l_closed 

    def connected_components(self):
        out = []
        l_open = set(self.dict.keys())
        
        while len(l_open) > 0:
            idx = l_open.pop()
            tmp = self.connected(idx)
            l_open -= tmp
            out.append(tmp)
        
        return out

    def __contains__(self, i):
        return self.dict[i]

    def __iter__(self):
        return iter(self.dict)

    def __len__(self):
        return len(self.dict)

    def __str__(self):
        return str(self.dict)


def build_graph(l, match_function):
    graph = SymmetricalUnweightedGraph()
    for i in range(len(l)):
        graph.add(i)
    for i in range(len(l)):
        for ii in range(i+1, len(l)):
            if match_function(l, i, ii):
                graph.connect(i, ii)
    return graph


def remove_duplicates(candidates):
    graph = build_graph(candidates, candidate_matcher)

    best_candidates = []
    for cc in graph.connected_components():
        best_sigma = 0
        best_idx = None
            
        for i in cc:
            if candidates[i].sigma >= best_sigma:
                best_idx = i
                best_sigma = candidates[i].sigma
                
        best_candidates.append(candidates[best_idx])
        
    return best_candidates

if __name__ == '__main__':
    import unittest

    class Candidate(object):
        def __init__(self, p, dm, z, sigma):
            self.p = p
            self.dm = dm
            self.z = z
            self.sigma = sigma

    class TestCandidateMatcher(unittest.TestCase):
        def test_match(self):
            '''Test whether a candidate matches itself.'''
            l = [Candidate(0.5, 10, 0, 100), Candidate(0.5, 10, 0, 100)]
            self.assertTrue(candidate_matcher(l, 0, 1))

        def test_no_match(self):
            '''Test whether different candidates don't match.'''
            l = [Candidate(0.5, 10, 0, 100), Candidate(2, 10, 0, 100)]
            self.assertFalse(candidate_matcher(l, 0, 1))
            l = [Candidate(0.5, 10, 0, 100), Candidate(2, 100, 0, 100)]
            self.assertFalse(candidate_matcher(l, 0, 1))
            l = [Candidate(0.5, 10, 0, 100), Candidate(0.5, 100, 0, 100)]
            self.assertFalse(candidate_matcher(l, 0, 1))

        def test_zero_dm(self):
            '''Test the case of a DM = 0 candidate.'''
            l = [Candidate(1, 0, 10, 100), Candidate(1, 0.01, 10, 100)]
            candidate_matcher(l, 0, 1)


    class TestBuildGraph(unittest.TestCase):
        def setUp(self):
            self.candidates = [
                Candidate(0, 0, 0, 100),
                Candidate(0, 0, 0, 100),
                Candidate(0, 0, 50, 100),
            ]

    class TestRejectDuplicates(unittest.TestCase):
        def test_no_duplicates(self):
            '''Test that a duplicates free list of candidates is not changed.'''
            l = remove_duplicates([Candidates(1, 1, 0, 100), Candidate(100, 100, 0, 100)])
            self.assertEqual(len(l), 2)

        def test_find_best(self):
            '''Test that best sigma is kept for candidates at equal p, dm.'''
            l = remove_duplicates([Candidate(1, 1, 0, 100), Candidate(1, 1, 0, 200),
                Candidate(1, 1, 0, 300)])
            self.assertEqual(l[0].sigma, 300)
 
    test_suite = unittest.TestSuite()
    test_suite.addTest(TestCandidateMatcher('test_match'))
    test_suite.addTest(TestCandidateMatcher('test_no_match'))
    test_suite.addTest(TestCandidateMatcher('test_zero_dm'))
    test_suite.addTest(TestRejectDuplicates('test_find_best'))

    unittest.TextTestRunner(verbosity=2).run(test_suite)
