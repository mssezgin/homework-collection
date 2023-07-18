# Mustafa Sezgin
# 2380863
# 
# Data Structures
# element:
#     constant: A
#     variable: x
#     function: (f, (element1..elementn))
# 
# predicate:
#     normal:  (True,  (p, (element1..elementn)))
#     negated: (False, (p, (element1..elementn)))
# 
# clause:
#     disjunction of predicates: (predicate1..predicaten)
# 
# set of clauses:
#     conjunction of clauses: {clause1..clausen}
# 
# clauses:
#     clauses with parents: {clause1: (parent11, parent12)..clausen: (parentn1, parentn2)}


def theorem_prover(kb, alpha):

    kb_set = {parse_clause(clause) for clause in set(kb)}
    kb_set = eliminate_tautologies(kb_set)
    kb_set = eliminate_subsumptions(kb_set)

    alpha_set = {parse_clause(clause) for clause in set(alpha)}
    alpha_set = eliminate_tautologies(alpha_set)
    alpha_set = eliminate_subsumptions(alpha_set)

    # initialize empty parents for known clauses
    clauses = {clause: ((), ()) for clause in kb_set.union(alpha_set)}
    new_clauses = {clause: ((), ()) for clause in alpha_set}

    while True:
        resolved_clauses = {}

        # TODO: use iterator function returning yield
        # iterate over all distinct pairs of clauses
        for clause1, clause2 in {frozenset((c1, c2)) for c1 in new_clauses for c2 in clauses if c1 is not c2}:

            # resolve clauses
            resolvents_set = resolve(clause1, clause2)
            resolvents = {resolvent: (clause1, clause2) for resolvent in resolvents_set}

            for key in resolvents:
                if key not in resolved_clauses:
                    resolved_clauses[key] = resolvents[key]

            # terminate if clauses resolve into empty clause (contradiction)
            if () in resolvents:
                break

        # update clauses with resolved clauses
        is_updated = False
        for key in resolved_clauses:
            if key not in clauses:
                is_updated = True
                clauses[key] = resolved_clauses[key]

        # update new clauses as resolved clauses
        new_clauses = resolved_clauses

        # terminate if clauses do not resolve into a new clause
        if not is_updated:
            return ("no", [])

        # terminate if clauses resolve into empty clause (contradiction)
        if () in clauses:
            parents = dict_of_parents(clauses, ())
            return ("yes", [clause_with_parents_to_string((parents[0], parents[1], resolvent)) for (resolvent, parents) in parents.items()])


def parse_clause(clause_string: str):
    clause = []

    # parse predicates
    for predicate_string in clause_string.split("+"):
        predicate = parse_predicate(predicate_string)
        clause.append(predicate)

    return tuple(clause)


def parse_predicate(predicate_string: str):
    # predicate = (True, ("", ()))

    # check if predicate is negated
    predicate_negation = True
    if predicate_string[0] == "~":
        predicate_negation = False
        predicate_string = predicate_string[1:]

    # parse predicate name and parameters
    predicate = parse_function(predicate_string)

    return (predicate_negation, predicate)


def parse_function(function_string: str):
    # function = ("", ())

    # parse function name
    function_name = function_string[:function_string.find("(")]

    # parse function parameters
    function_params = []
    function_param_strings = function_string[function_string.find("(") + 1:function_string.rfind(")")].split(",")
    n_parenthesis = 0
    i_start_parenthesis = -1
    for i, function_param_string in enumerate(function_param_strings):

        # count open parenthesis for parameters as functions
        n_open_parenthesis = function_param_string.count("(")
        if n_open_parenthesis > 0:
            if n_parenthesis == 0:
                i_start_parenthesis = i
            n_parenthesis += n_open_parenthesis

        # count close parenthesis and parse recursively
        n_close_parenthesis = function_param_string.count(")")
        if n_close_parenthesis > 0:
            n_parenthesis -= n_close_parenthesis
            if n_parenthesis == 0: # and i_start_parenthesis > 0:
                function_param = parse_function(",".join(function_param_strings[i_start_parenthesis:i+1]))
                function_params.append(function_param)
                i_start_parenthesis = -1

        # parameter is constant or variable
        if n_open_parenthesis == 0 and n_close_parenthesis == 0:
            function_params.append(function_param_string)

    return (function_name, tuple(function_params))


def clause_with_parents_to_string(clause_with_parents: tuple):
    return "$".join(map(clause_to_string, clause_with_parents))


def clause_to_string(clause: tuple):
    if clause:
        return "+".join(map(predicate_to_string, clause))
    else:
        return "empty"


def predicate_to_string(predicate: tuple):
    # check if predicate is negated
    if not predicate[0]:
        predicate_string_prefix = "~"
    else:
        predicate_string_prefix = ""

    return predicate_string_prefix + function_to_string(predicate[1])


def function_to_string(function: tuple):
    return f"{function[0]}({','.join(map(function_param_to_string, function[1]))})"


def function_param_to_string(function_param):
    if type(function_param) is tuple:
        return function_to_string(function_param)
    else:
        return function_param


def eliminate_tautologies(clauses_set: set):
    eliminated_clauses_set = clauses_set.copy()
    for clause in clauses_set:
        if is_tautology(clause):
            eliminated_clauses_set.remove(clause)
    return eliminated_clauses_set


def is_tautology(clause: tuple):
    for predicate1, predicate2 in {frozenset((p1, p2)) for p1 in clause for p2 in clause if p1 is not p2}:
        if predicate1[0] != predicate2[0] and unify(predicate1[1], predicate2[1])[0]:
            return True
    return False


def eliminate_subsumptions(clauses_set: set):
    eliminated_clauses_set = clauses_set.copy()
    for clause1, clause2 in {(c1, c2) for c1 in clauses_set for c2 in clauses_set if c1 is not c2}:
        if does_subsume(clause1, clause2):
            eliminated_clauses_set.remove(clause2)
    return eliminated_clauses_set


def does_subsume(clause1: tuple, clause2: tuple):
    # TODO does not work properly
    # for predicate1, predicate2 in [(p1, p2) for p1 in clause1 for p2 in clause2]:
    # 
    #     if predicate1[0] != predicate2[0]:
    #         continue
    # 
    #     unifiable, substitution = unify(predicate1[1], predicate2[1])
    #     if not unifiable:
    #         continue
    # 
    #     for predicate in clause1:
    #         if predicate is not predicate1 and substitute(predicate, substitution) not in clause2:
    #             break
    #     else:
    #         return True
    # 
    # return False
    return False


def resolve(clause1: tuple, clause2: tuple):
    resolvents = set()

    for predicate1, predicate2 in [(p1, p2) for p1 in clause1 for p2 in clause2]:

        # predicaes must be inverse in terms of negation
        if predicate1[0] == predicate2[0]:
            continue

        # try to unify predicates
        (unifiable, substitution) = unify(predicate1[1], predicate2[1])

        # predicates must be unifiable
        if not unifiable:
            continue

        # resolve and apply substitution
        resolvent = []
        for resolved_predicate in clause1:
            if resolved_predicate is not predicate1:
                resolvent.append(substitute(resolved_predicate, substitution))
        for resolved_predicate in clause2:
            if resolved_predicate is not predicate2:
                resolvent.append(substitute(resolved_predicate, substitution))

        resolvents.add(tuple(resolvent))

    return resolvents


def unify(element1, element2):

    if is_constant(element1):

        if is_constant(element2):
            if element1 == element2:
                return (True, {})
            else:
                return (False, {})

        if is_variable(element2):
            return (True, {element2: element1})

        if is_function(element2):
            return (False, {})

    if is_variable(element1):

        if is_constant(element2):
            return (True, {element1: element2})

        if is_variable(element2):
            if element1 == element2:
                return (True, {})
            else:
                return (True, {element1: element2})

        if is_function(element2):
            if is_variable_in_function(element1, element2):
                return (False, {})
            else:
                return (True, {element1: element2})

    if is_function(element1):

        if is_constant(element2):
            return (False, {})

        if is_variable(element2):
            if is_variable_in_function(element2, element1):
                return (False, {})
            else:
                return (True, {element2: element1})

        if is_function(element2):
            if element1[0] != element2[0] or len(element1[1]) != len(element2[1]):
                return (False, {})

            substitution = {}
            for function_param1, function_param2 in zip(element1[1], element2[1]):
                function_param1_substituted = substitute(function_param1, substitution)
                function_param2_substituted = substitute(function_param2, substitution)
                param_unifiable, param_substitution = unify(function_param1_substituted, function_param2_substituted)
                if not param_unifiable:
                    return (False, {})
                compose_substitutions(substitution, param_substitution)

            return (True, substitution)

    return (False, {})


def is_constant(element):
    return type(element) is str and element[0].isupper()


def is_variable(element):
    return type(element) is str and element[0].islower()


def is_function(element):
    return type(element) is tuple and len(element) == 2 and is_variable(element[0]) and type(element[1]) is tuple


def is_predicate(element):
    return type(element) is tuple and len(element) == 2 and type(element[0]) is bool and is_function(element[1])


def is_variable_in_function(variable: str, function: tuple):
    if variable in function:
        return True

    for function_param in function[1]:

        if is_variable(function_param) and variable == function_param:
            return True

        if is_function(function_param) and is_variable_in_function(variable, function_param):
            return True

    return False


def compose_substitutions(substitution1: dict, substitution2: dict):
    for key in substitution1:
        substitution1[key] = substitute(substitution1[key], substitution2)

    for key in substitution2:
        if key not in substitution1:
            substitution1[key] = substitution2[key]

def substitute(element, substitution: dict):

    if is_constant(element):
        return element

    if is_variable(element):
        if element in substitution:
            return substitution[element]
        else:
            return element

    if is_function(element):
        return (element[0], tuple(substitute(function_param, substitution) for function_param in element[1]))

    if is_predicate(element):
        return (element[0], substitute(element[1], substitution))

    return element


def dict_of_parents(clauses: dict, child_clause: tuple):
    parent_clauses = clauses[child_clause]
    if parent_clauses == ((), ()):
        return {}

    parents = {}
    parents.update(dict_of_parents(clauses, parent_clauses[0]))
    parents.update(dict_of_parents(clauses, parent_clauses[1]))
    parents[child_clause] = clauses[child_clause]
    return parents
