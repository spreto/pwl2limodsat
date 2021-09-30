#!/usr/bin/python3

import os
import sys
import subprocess
import math
import fractions
import random
import threading
import copy

#   settings   ########################################################################################################
#######################################################################################################################

data_folder = "./"
yices = "yices-smt2"
pwl2limodsat_path = "../../bin/Release/pwl2limodsat"

# If tl_test is non empty, such function will be the only one tested
tl_test = []
# tl_test = [4, 9, 0, 1, 2, 3]
# tl_test = [5, 6, 0, 1, -1, 2]
# tl_test = [4, 3, -1, 1, 0, 1]

# If tl_test is empty and pwl_test is non empty, such function will be the only one tested
pwl_test = []
# pwl_test = [
#     [ [4, 9, 0, 1, 2, 3], [5, 6, 0, 1, -1, 2], [4, 3, -1, 1, 0, 1] ],
#     [ [8, -9, -6], [1, -2, 1], [0.333333, 0, -1], [0, 1, 0], [1, -1, 0], [0, 0, 1], [1, 0, -1] ],
#     [
#         [ ['g', 0], ['g', 2], ['g', 3], ['g', 5] ],
#         [ ['g', 1], ['l', 2], ['g', 3], ['g', 6] ],
#         [ ['l', 0], ['l', 1], ['g', 4], ['g', 5] ]
#     ],
# ]

# If tl_test and pwl_test are empty, random tests of type TEST_TYPE will be performed
# 0: random truncated linear
# 1: normalized random linear
# 2: simple region piecewise linear
# 3: cubic region piecewise linear
TEST_TYPE = 1
MAXDIMENSIONSTHREADING = 1

NUMEVALUATIONS = 100
DECPRECISION = 6
DECPRECISION_form = ".6f"

# Used for types 0 and 1
MAXDIMENSION = 50
NUMTESTSBYCONFIG = 100
MAXINTEGER = 100

# Used for type 2
# MAXDIMENSION = 50
# NUMTESTSBYCONFIG = 1
# MAXINTEGER = 100
# MAXREGIONALPARAM = 20

# Used for type 3
# MAXDIMENSION = 9
# NUMTESTSBYCONFIG = 30
# MAXINTEGER = 30
# MAXREGIONALPARAM = 7

#   create tl instance   ##############################################################################################
#######################################################################################################################

def createTl(instance_data, pwl_file_name):
    pwl_file = open(data_folder+pwl_file_name, "w")
    pwl_file.write("tl ")
    for coeff in range(len(instance_data)):
        pwl_file.write(str(instance_data[coeff]))
        if coeff != len(instance_data)-1:
            pwl_file.write(" ")
    pwl_file.close()

#   create pwl instance   #############################################################################################
#######################################################################################################################

def createPwl(instance_data, pwl_file_name, instance_dimension):
    pwl_file = open(data_folder+pwl_file_name, "w")

    pwl_file.write("pwl\n\n")

    for b in range(len(instance_data[1])):
        pwl_file.write("b ")
        for bc in range(instance_dimension+1):
            pwl_file.write(str(instance_data[1][b][bc]))
            if bc != instance_dimension:
                pwl_file.write(" ")
        pwl_file.write("\n")

    pwl_file.write("\n")

    for p in range(len(instance_data[0])):
        pwl_file.write("p ")
        for c in range(2*(instance_dimension+1)):
            pwl_file.write(str(instance_data[0][p][c]))
            if c != 2*instance_dimension+1:
                pwl_file.write(" ")
        pwl_file.write("\n")
        for b in range(len(instance_data[2][p])):
            pwl_file.write(instance_data[2][p][b][0]+" "+str(instance_data[2][p][b][1]+1)+"\n")
        pwl_file.write("\n")

    pwl_file.close()

#   create limodsat instance   ########################################################################################
#######################################################################################################################

def pwl2out(pwl_file_name):
    os.system(pwl2limodsat_path+" "+data_folder+pwl_file_name)

#   generate random values   ##########################################################################################
#######################################################################################################################

def generateValues(instance_dimension):
    propositional_variables_values = []

    for value in range(instance_dimension):
        propositional_variables_values.append(round(random.uniform(0,1), DECPRECISION))

    return propositional_variables_values

#   create smt instance   #############################################################################################
#######################################################################################################################

def createSmt(out_file_name, smt_file_name, instance_dimension, propositional_variables_values):
    formula = []
    maxvar = instance_dimension
    phi = False
    smt_aux = []

    out_file = open(data_folder+out_file_name, "r")

    for line in out_file:
        if "Unit" == line[0:4]:
            if ":: Clause" == line[line.find("::"):line.find("::")+9]:
                linepos_begin = line.find("::")+18
                linepos_end = linepos_begin+line[linepos_begin:].find(" ")

                if int(line[linepos_begin:linepos_end]) > 0:
                    clau = "X"+line[linepos_begin : linepos_end]
                    maxvar = max([maxvar,int(line[linepos_begin : linepos_end])])
                else:
                    clau = "(neg "+"X"+line[linepos_begin+1:linepos_end]+")"
                    maxvar = max([maxvar,int(line[linepos_begin+1:linepos_end])])

                while linepos_end+1 < len(line)-1:
                    linepos_begin = linepos_end+1
                    linepos_end = linepos_begin+line[linepos_begin:].find(" ")

                    if int(line[linepos_begin:linepos_end]) > 0:
                        clau = "(sdis "+clau+" X"+line[linepos_begin : linepos_end]+")"
                        maxvar = max([maxvar,int(line[linepos_begin : linepos_end])])
                    else:
                        clau = "(sdis "+clau+" (neg X"+line[linepos_begin+1 : linepos_end]+"))"
                        maxvar = max([maxvar,int(line[linepos_begin+1 : linepos_end])])

                formula.append(clau)

            elif ":: Negation" == line[line.find("::"):line.find("::")+11]:
                linepos_begin = line.find("::")+18
                linepos_end = len(line)-1

                formula.append("(neg "+formula[int(line[linepos_begin : linepos_end])-1]+")")

            elif ":: Implication" == line[line.find("::"):line.find("::")+14]:
                linepos_begin = line.find("::")+18
                linepos_end = linepos_begin+line[linepos_begin:].find(" ")
                linepos_begin2 = linepos_end+1
                linepos_end2 = len(line)-1

                formula.append("(impl "+formula[int(line[linepos_begin:linepos_end])-1]+" "+formula[int(line[linepos_begin2:linepos_end2])-1]+")")

            elif ":: Equivalence" == line[line.find("::"):line.find("::")+14]:
                linepos_begin = line.find("::")+18
                linepos_end = linepos_begin+line[linepos_begin:].find(" ")
                linepos_begin2 = linepos_end+1
                linepos_end2 = len(line)-1

                formula.append("(equiv "+formula[int(line[linepos_begin:linepos_end])-1]+" "+formula[int(line[linepos_begin2:linepos_end2])-1]+")")

            elif ":: Minimum" == line[line.find("::"):line.find("::")+10]:
                linepos_begin = line.find("::")+18
                linepos_end = linepos_begin+line[linepos_begin:].find(" ")
                linepos_begin2 = linepos_end+1
                linepos_end2 = len(line)-1

                formula.append("(min "+formula[int(line[linepos_begin:linepos_end])-1]+" "+formula[int(line[linepos_begin2:linepos_end2])-1]+")")

            elif ":: Maximum" == line[line.find("::"):line.find("::")+10]:
                linepos_begin = line.find("::")+18
                linepos_end = linepos_begin+line[linepos_begin:].find(" ")
                linepos_begin2 = linepos_end+1
                linepos_end2 = len(line)-1

                formula.append("(max "+formula[int(line[linepos_begin:linepos_end])-1]+" "+formula[int(line[linepos_begin2:linepos_end2])-1]+")")

        else:
            if formula:
                if not phi:
                    smt_aux.append("(assert (= phi "+formula[len(formula)-1]+"))")
                    phi = True
                else:
                    smt_aux.append("(assert (= "+formula[len(formula)-1]+" 1))")

                formula = []

    if formula:
        smt_aux.append("(assert (= "+formula[len(formula)-1]+" 1))")

    out_file.close()

    smt_file = open(data_folder+smt_file_name, "w")

    smt_file.write("(set-logic QF_LRA)"+"\n")
    smt_file.write("(define-fun min ((x Real) (y Real)) Real(ite (> x y) y x))"+"\n")
    smt_file.write("(define-fun max ((x Real) (y Real)) Real(ite (> x y) x y))"+"\n")
    smt_file.write("(define-fun sdis ((x Real) (y Real)) Real(min 1 (+ x y)))"+"\n")
    smt_file.write("(define-fun scon ((x Real) (y Real)) Real(max 0 (- (+ x y) 1)))"+"\n")
    smt_file.write("(define-fun wdis ((x Real) (y Real)) Real(max x y))"+"\n")
    smt_file.write("(define-fun wcon ((x Real) (y Real)) Real(min y x))"+"\n")
    smt_file.write("(define-fun neg ((x Real)) Real(- 1 x))"+"\n")
    smt_file.write("(define-fun impl ((x Real) (y Real)) Real(min 1 (- (+ 1 y) x)))"+"\n")
    smt_file.write("(define-fun equiv ((x Real) (y Real)) Real(- 1 (max (- x y) (- y x))))"+"\n")
    smt_file.write("\n")
    smt_file.write("(declare-fun phi () Real)"+"\n")

    for var in range(1,maxvar+1):
        smt_file.write("(declare-fun X"+str(var)+" () Real)"+"\n")

    smt_file.write("\n")

    for var in range(instance_dimension+1,maxvar+1):
        smt_file.write("(assert (>= X"+str(var)+" 0))"+"\n")
        smt_file.write("(assert (<= X"+str(var)+" 1))"+"\n")

    smt_file.write("\n")

    for string in smt_aux:
        smt_file.write(string+"\n")

    smt_file.write("\n")

    for val in range(len(propositional_variables_values)):
        smt_file.write("(assert (= X"+str(val+1)+" "+format(propositional_variables_values[val], DECPRECISION_form)+"))"+"\n")

    smt_file.write("\n")
    smt_file.write("(check-sat)")
    smt_file.write("\n")
    smt_file.write("(get-value (phi))")

    smt_file.close()

#   evaluate truncated linear   #######################################################################################
#######################################################################################################################

def evaluateTl(instance_data, propositional_variables_values):
    evaluation = instance_data[0] / instance_data[1]

    for val in range(len(propositional_variables_values)):
        evaluation += ( instance_data[ 2*val+2 ] / instance_data[ 2*val+3 ] ) * propositional_variables_values[val]

    evaluation = min([1,max([0,evaluation])])

    return evaluation

#   evaluate piecewise linear   #######################################################################################
#######################################################################################################################

def evaluatePwl(instance_data, propositional_variables_values, instance_dimension):
    boundaryIndex = -1
    activeRegion = False

    while activeRegion == False:
        boundaryIndex += 1
        activeRegion = True
        for bound in instance_data[2][boundaryIndex]:
            phi = instance_data[1][bound[1]][0]
            for i in range(instance_dimension):
                phi += propositional_variables_values[i]*instance_data[1][bound[1]][i+1]
            if not ((phi >= 0 and bound[0] == 'g') or (phi <= 0 and bound[0] == 'l')):
                activeRegion = False

    evaluation = instance_data[0][boundaryIndex][0]/instance_data[0][boundaryIndex][1]
    for val in range(2,2*(instance_dimension+1),2):
        evaluation += (instance_data[0][boundaryIndex][val]/instance_data[0][boundaryIndex][val+1])*propositional_variables_values[int(val/2)-1]

    return evaluation

#   evaluate smt instance   ###########################################################################################
#######################################################################################################################

def evaluateSmt(smt_file_name):
    smt_out = subprocess.check_output([yices, data_folder+smt_file_name]).decode(sys.stdout.encoding)

    if smt_out[smt_out.find("phi")+4 : smt_out.find("phi")+6] != "(/":
        evaluation = int(smt_out[smt_out.find("phi")+4 : smt_out.find(")")])
    else:
        beginpos = smt_out.find("phi")+7
        endpos = beginpos+smt_out[beginpos:].find(" ")
        beginpos2 = endpos+1
        endpos2 = beginpos2+smt_out[beginpos2:].find(")")
        evaluation = int(smt_out[beginpos : endpos]) / int(smt_out[beginpos2 : endpos2])

    return evaluation

#   compare values   ##################################################################################################
#######################################################################################################################

def valuesMatch(first_value, second_value):
    if abs(first_value - second_value) < 10**-DECPRECISION:
        return True
    else:
        return False

#   test truncated linear   ###########################################################################################
#######################################################################################################################

def testTl(instance_data, test_file_name, instance_dimension):
    global summary

    createTl(instance_data, test_file_name+".pwl")
    pwl2out(test_file_name+".pwl")

    results = []
    stat = [0,0]

    for eval_num in range(NUMEVALUATIONS):
        values = generateValues(instance_dimension)
#        createSmt(test_file_name+".out", test_file_name+"_"+str(eval_num)+".smt", instance_dimension, values)
        createSmt(test_file_name+".limodsat", test_file_name+"_"+str(eval_num)+".smt", instance_dimension, values)

        evaluation = evaluateTl(instance_data, values)
        evaluationModSat = evaluateSmt(test_file_name+"_"+str(eval_num)+".smt")

        single_result = ""

        for dim in range(instance_dimension):
            single_result += "X"+str(dim+1)+": "+str(values[dim])+" | "

        single_result += "| eval: "+str(evaluation)+" | evalModSat: "+str(evaluationModSat)

        if valuesMatch(evaluation, evaluationModSat):
            single_result += " || SUCCESS :)"
            stat[0] += 1
        else:
            single_result += " || fail :("
            stat[1] += 1

        results.append(single_result)
        os.system("rm "+data_folder+test_file_name+"_"+str(eval_num)+".smt")

    results_file = open(data_folder+test_file_name+".res", "w")

    if not stat[1]:
        results_file.write("PASSED ALL EVALUATIONS!!!")
    elif not stat[0]:
        results_file.write("FAILED all evaluations :(")
    else:
        results_file.write("PASSED: "+str(stat[0])+" | failed: "+str(stat[1]))

    results_file.write("\n\n")

    for res in range(len(results)):
        results_file.write(results[res])
        if res != len(results)-1:
            results_file.write("\n")

    results_file.close()

    summary.append([test_file_name, stat])

#   test piecewise linear   ###########################################################################################
#######################################################################################################################

def testPwl(instance_data, test_file_name, instance_dimension):
    global summary

    createPwl(instance_data, test_file_name+".pwl", instance_dimension)
    pwl2out(test_file_name+".pwl")

    results = []
    stat = [0,0]

    for eval_num in range(NUMEVALUATIONS):
        values = generateValues(instance_dimension)
#        createSmt(test_file_name+".out", test_file_name+"_"+str(eval_num)+".smt", instance_dimension, values)
        createSmt(test_file_name+".limodsat", test_file_name+"_"+str(eval_num)+".smt", instance_dimension, values)

        evaluation = evaluatePwl(instance_data, values, instance_dimension)
        evaluationModSat = evaluateSmt(test_file_name+"_"+str(eval_num)+".smt")

        single_result = ""

        for dim in range(instance_dimension):
            single_result += "X"+str(dim+1)+": "+str(values[dim])+" | "

        single_result += "| eval: "+str(evaluation)+" | evalModSat: "+str(evaluationModSat)

        if valuesMatch(evaluation, evaluationModSat):
            single_result += " || SUCCESS :)"
            stat[0] += 1
        else:
            single_result += " || fail :("
            stat[1] += 1

        results.append(single_result)
        os.system("rm "+data_folder+test_file_name+"_"+str(eval_num)+".smt")

    results_file = open(data_folder+test_file_name+".res", "w")

    if not stat[1]:
        results_file.write("PASSED ALL EVALUATIONS!!!")
    elif not stat[0]:
        results_file.write("FAILED all evaluations :(")
    else:
        results_file.write("PASSED: "+str(stat[0])+" | failed: "+str(stat[1]))

    results_file.write("\n\n")

    for res in range(len(results)):
        results_file.write(results[res])
        if res != len(results)-1:
            results_file.write("\n")

    results_file.close()

    summary.append([test_file_name, stat])

#   generate truncated linear function   ##############################################################################
#######################################################################################################################

def generateTl(instance_dimension):
    instance_data = []

    for c in range(instance_dimension+1):
        instance_data.append(random.randint(-MAXINTEGER,MAXINTEGER))
        instance_data.append(random.randint(1,MAXINTEGER))

    return instance_data

#   generate normalized linear function   #############################################################################
#######################################################################################################################

def generateTl_norm(instance_dimension):
    instance_data = []

    for c in range(instance_dimension+1):
        instance_data.append(random.randint(-MAXINTEGER,MAXINTEGER))
        instance_data.append(random.randint(1,MAXINTEGER))

    minimum = 0
    for c in range(2,2*(instance_dimension+1),2):
        if instance_data[c] < 0:
            minimum += instance_data[c]/instance_data[c+1]
    minimum += instance_data[0]/instance_data[1]

    if minimum < 0:
        num = 1
        while num/instance_data[1] < abs(minimum):
            num += 1
        instance_data[0] += num

    maximum = 0
    for c in range(2,2*(instance_dimension+1),2):
        if instance_data[c] > 0:
            maximum += instance_data[c]/instance_data[c+1]
    maximum += instance_data[0]/instance_data[1]

    if maximum > 1:
        for c in range(0,2*(instance_dimension+1),2):
            instance_data[c+1] *= math.ceil(maximum)

    return instance_data

#   generate simple region piecewise linear function   ################################################################
#######################################################################################################################

def generatePwl_simp(instance_dimension, regional_parameter):
    coefficients = []

    for c in range(instance_dimension+1):
        coefficients.append(random.randint(-MAXINTEGER,MAXINTEGER))
        coefficients.append(random.randint(1,MAXINTEGER))

    minimum = 0
    for c in range(4,2*(instance_dimension+1),2):
        if coefficients[c] < 0:
            minimum += coefficients[c]/coefficients[c+1]
    if instance_dimension > 0 and coefficients[2] < 0:
        minimum += (coefficients[2]*(1/regional_parameter)) / coefficients[3]
    minimum += coefficients[0]/coefficients[1]

    if minimum < 0:
        num = 1
        while num/coefficients[1] < abs(minimum):
            num += 1
        coefficients[0] += num

    maximum = 0
    for c in range(4,2*(instance_dimension+1),2):
        if coefficients[c] > 0:
            maximum += coefficients[c]/coefficients[c+1]
    if instance_dimension > 0 and coefficients[2] > 0:
        maximum += (coefficients[2]*(1/regional_parameter)) / coefficients[3]
    maximum += coefficients[0]/coefficients[1]

    if maximum > 1:
        for c in range(0,2*(instance_dimension+1),2):
            coefficients[c+1] *= math.ceil(maximum)

    boundary_prot = []

    for b in range(regional_parameter+1):
        bound = [0]*(instance_dimension+1)
        bound[0] = -b*(1/regional_parameter)
        bound[1] = 1
        boundary_prot.append(bound)

    for b in range(2,instance_dimension+1):
        bound = [0]*(instance_dimension+1)
        bound[0] = 0
        bound[b] = 1
        boundary_prot.append(bound)
        bound = [0]*(instance_dimension+1)
        bound[0] = -1
        bound[b] = 1
        boundary_prot.append(bound)

    function = []
    function.append(coefficients)

    boundary = []
    bound = []
    bound.append(['g', 0])
    bound.append(['l', 1])
    for b in range(regional_parameter+1, len(boundary_prot), 2):
        bound.append(['g', b])
        bound.append(['l', b+1])
    boundary.append(bound)

    for r in range(1,regional_parameter):
        coefficients = []
        for c in range(2*(instance_dimension+1)):
            coefficients.append(function[len(function)-1][c])

        minimum = 0
        for c in range(4,2*(instance_dimension+1),2):
            if coefficients[c] < 0:
                minimum += coefficients[c]/coefficients[c+1]
        minimum += (coefficients[2]*((r+1)/regional_parameter)) / coefficients[3]
        minimum += coefficients[0]/coefficients[1]

        maximum = 0
        for c in range(4,2*(instance_dimension+1),2):
            if coefficients[c] > 0:
                maximum += coefficients[c]/coefficients[c+1]
        maximum += (coefficients[2]*((r+1)/regional_parameter)) / coefficients[3]
        maximum += coefficients[0]/coefficients[1]

        rand = round(random.uniform(-minimum*regional_parameter, (1-maximum)*regional_parameter),int(math.log10(MAXINTEGER))+1)
        q = fractions.Fraction.from_float(rand).limit_denominator(10**int((math.log10(MAXINTEGER)+1)))

        aux = fractions.Fraction(coefficients[0],coefficients[1]) + q*fractions.Fraction(-r,regional_parameter)
        coefficients[0] = aux.numerator
        coefficients[1] = aux.denominator

        aux = fractions.Fraction(coefficients[2],coefficients[3]) + q
        coefficients[2] = aux.numerator
        coefficients[3] = aux.denominator

        function.append(coefficients)

        bound = []
        bound.append(['g', r])
        bound.append(['l', r+1])
        for b in range(regional_parameter+1, len(boundary_prot), 2):
            bound.append(['g', b])
            bound.append(['l', b+1])
        boundary.append(bound)

    instance_data = []
    instance_data.append(function)
    instance_data.append(boundary_prot)
    instance_data.append(boundary)

    return instance_data

#   generate cubic region piecewise linear function   #################################################################
#######################################################################################################################

def generatePwl_cub(instance_dimension, regional_parameter):
    coefficients = []

    for c in range(instance_dimension+1):
        coefficients.append(random.randint(-MAXINTEGER,MAXINTEGER))
        coefficients.append(random.randint(1,MAXINTEGER))

    minimum = 0
    for c in range(2,2*(instance_dimension+1),2):
        if coefficients[c] < 0:
            minimum += (coefficients[c]*(1/regional_parameter)) / coefficients[c+1]
    minimum += coefficients[0]/coefficients[1]

    if minimum < 0:
        num = 1
        while num/coefficients[1] < abs(minimum):
            num += 1
        coefficients[0] += num

    maximum = 0
    for c in range(2,2*(instance_dimension+1),2):
        if coefficients[c] > 0:
            maximum += (coefficients[c]*(1/regional_parameter)) / coefficients[c+1]
    maximum += coefficients[0]/coefficients[1]

    if maximum > 1:
        for c in range(0,2*(instance_dimension+1),2):
            coefficients[c+1] *= math.ceil(maximum)

    boundary_prot = []

    for b0 in range(1,instance_dimension+1):
        for b in range(regional_parameter+1):
            bound = [0]*(instance_dimension+1)
            bound[0] = -b*(1/regional_parameter)
            bound[b0] = 1
            boundary_prot.append(bound)

    function = []
    function.append(coefficients)

    boundary = []
    bound = []
    for b in range(0, len(boundary_prot), regional_parameter+1):
        bound.append(['g', b])
        bound.append(['l', b+1])
    boundary.append(bound)

    fIndex = []
    fIndex.append([0]*instance_dimension)

    f = [0]*instance_dimension
    f[0] = 1
    allDefined = 0

    if regional_parameter == 1:
        allDefined = 1

    globalMinimum = 0
    globalMaximum = 1

    while not allDefined:
        i = 0
        while f[i] == 0:
            i = i+1
        fAIndex = copy.deepcopy(f)
        fAIndex[i] = fAIndex[i] - 1
        r = i

        for j in range(len(fIndex)):
            if fIndex[j] == fAIndex:
                fA = j

        coefficients = []
        for c in range(2*(instance_dimension+1)):
            coefficients.append(function[fA][c])

        i = i+1
        while i < instance_dimension and f[i] == 0:
            i = i+1

        if i < instance_dimension:
            fBIndex = copy.deepcopy(f)
            fBIndex[i] = fBIndex[i] - 1

            fB = 0
            for j in range(len(fIndex)):
                if fIndex[j] == fBIndex:
                    fB = j

            qAux = (function[fB][2*r+2] / function[fB][2*r+3]) - (function[fA][2*r+2] / function[fA][2*r+3])
            q = fractions.Fraction.from_float(qAux).limit_denominator(10**int((math.log10(MAXINTEGER)+1)))

            aux = fractions.Fraction(coefficients[0],coefficients[1]) + q*fractions.Fraction(-f[r],regional_parameter)
            coefficients[0] = aux.numerator
            coefficients[1] = aux.denominator

            aux = fractions.Fraction(coefficients[2*r+2],coefficients[2*r+3]) + q
            coefficients[2*r+2] = aux.numerator
            coefficients[2*r+3] = aux.denominator
        else:
            minimum = coefficients[0]/coefficients[1]
            for c in range(2,2*(instance_dimension+1),2):
                if coefficients[c] < 0:
                    minimum += ( coefficients[c]*((f[int(c/2)-1]+1)/regional_parameter) ) / coefficients[c+1]
                else:
                    minimum += ( coefficients[c]*((f[int(c/2)-1])/regional_parameter) ) / coefficients[c+1]

            maximum = coefficients[0]/coefficients[1]
            for c in range(2,2*(instance_dimension+1),2):
                if coefficients[c] > 0:
                    maximum += ( coefficients[c]*((f[int(c/2)-1]+1)/regional_parameter) ) / coefficients[c+1]
                else:
                    maximum += ( coefficients[c]*((f[int(c/2)-1])/regional_parameter) ) / coefficients[c+1]

            rand = round(random.uniform(-minimum*regional_parameter, (1-maximum)*regional_parameter),int(math.log10(MAXINTEGER))+1)
            q = fractions.Fraction.from_float(rand).limit_denominator(10**int((math.log10(MAXINTEGER)+1)))

            aux = fractions.Fraction(coefficients[0],coefficients[1]) + q*fractions.Fraction(-f[r],regional_parameter)
            coefficients[0] = aux.numerator
            coefficients[1] = aux.denominator

            aux = fractions.Fraction(coefficients[2*r+2],coefficients[2*r+3]) + q
            coefficients[2*r+2] = aux.numerator
            coefficients[2*r+3] = aux.denominator

        function.append(coefficients)

        minimum = coefficients[0]/coefficients[1]
        for c in range(2,2*(instance_dimension+1),2):
            if coefficients[c] < 0:
                minimum += ( coefficients[c]*((f[int(c/2)-1]+1)/regional_parameter) ) / coefficients[c+1]
            else:
                minimum += ( coefficients[c]*((f[int(c/2)-1])/regional_parameter) ) / coefficients[c+1]

        if minimum < globalMinimum:
            globalMinimum = minimum

        maximum = coefficients[0]/coefficients[1]
        for c in range(2,2*(instance_dimension+1),2):
            if coefficients[c] > 0:
                maximum += ( coefficients[c]*((f[int(c/2)-1]+1)/regional_parameter) ) / coefficients[c+1]
            else:
                maximum += ( coefficients[c]*((f[int(c/2)-1])/regional_parameter) ) / coefficients[c+1]

        if maximum > globalMaximum:
            globalMaximum = maximum

        bound = []
        for b in range(instance_dimension):
            bound.append(['g', b*(regional_parameter+1)+f[b]])
            bound.append(['l', b*(regional_parameter+1)+f[b]+1])
        boundary.append(bound)

        fIndex.append(copy.deepcopy(f))

        if f == [regional_parameter-1]*instance_dimension:
            allDefined = 1
        else:
            i = 0
            while f[i] == regional_parameter-1:
                f[i] = 0
                i = i+1
            f[i] = f[i] + 1

    if globalMinimum < 0:
        minAux = fractions.Fraction.from_float(abs(globalMinimum)).limit_denominator(10**int((math.log10(MAXINTEGER)+1)))
        for func in function:
            aux = fractions.Fraction(func[0], func[1]) + minAux
            func[0] = aux.numerator
            func[1] = aux.denominator

    if globalMaximum+abs(globalMinimum) > 1:
        maxAux = 1
        while maxAux < globalMaximum+abs(globalMinimum):
            maxAux = maxAux + 1
        for func in function:
            for c in range(1,2*(instance_dimension+1)+1,2):
                func[c] = func[c]*maxAux

    instance_data = []
    instance_data.append(function)
    instance_data.append(boundary_prot)
    instance_data.append(boundary)

    return instance_data

#   create summary   ##################################################################################################
#######################################################################################################################

def createSummary():
    global summary

    final_analysis = "PASSED ALL TESTS!!! :D"

    for sum in summary:
        if (sum[1])[1] != 0:
            final_analysis = "Failed in at least one test... :("

    summary_file = open(data_folder+"summary.res", "w")

    summary_file.write(final_analysis)
    summary_file.write("\n\n")

    for sum in summary:
        summary_file.write(sum[0]+" | PASSED: "+str((sum[1])[0])+" | Failed: "+str((sum[1])[1])+"\n")

    summary_file.close()

#   runner for truncated linear tests   ###############################################################################
#######################################################################################################################

def testRunnerTl(genType, testName):
    thr_test = []

    for dim in range(1,MAXDIMENSION+1):
        for test_num in range(NUMTESTSBYCONFIG):
            print("Dimension: "+str(dim)+" | Test num. "+str(test_num+1)+"\n")
            thr_test.append(threading.Thread(target=testTl, args=(genType(dim), testName+"_"+str(dim)+"_"+str(test_num+1), dim)))
            thr_test[-1].start()

        if dim % MAXDIMENSIONSTHREADING == 0 or dim == MAXDIMENSION:
            finished = False
            finished_thr_test = [False]*len(thr_test)

            while not finished:
                for th in range(len(thr_test)):
                    if not thr_test[th].is_alive():
                        finished_thr_test[th] = True

                finished = True
                for th in range(len(finished_thr_test)):
                    if not finished_thr_test[th]:
                        finished = False

            thr_test = []

#   runner for piecewise linear tests   ###############################################################################
#######################################################################################################################

def testRunnerPwl(genType, testName):
    thr_test = []

    for dim in range(1,MAXDIMENSION+1):
        for param in range(1,MAXREGIONALPARAM+1):
            for test_num in range(NUMTESTSBYCONFIG):
                print("Dimension: "+str(dim)+" | Regional parameter: "+str(param)+" | Test num. "+str(test_num+1)+"\n")
                thr_test.append(threading.Thread(target=testPwl, args=(genType(dim,param), testName+"_"+str(dim)+"_"+str(param)+"_"+str(test_num+1), dim)))
                thr_test[-1].start()

        if dim % MAXDIMENSIONSTHREADING == 0 or dim == MAXDIMENSION:
            finished = False
            finished_thr_test = [False]*len(thr_test)

            while not finished:
                for th in range(len(thr_test)):
                    if not thr_test[th].is_alive():
                        finished_thr_test[th] = True

                finished = True
                for th in range(len(finished_thr_test)):
                    if not finished_thr_test[th]:
                        finished = False

            thr_test = []

#   runner 2 for piecewise linear tests   #############################################################################
#######################################################################################################################

def testRunnerPwl2(genType, testName):
    thr_test = []

    regions = MAXREGIONALPARAM

    for dim in range(1,MAXDIMENSION+1):
        for param in range(1,regions+1):
            for test_num in range(NUMTESTSBYCONFIG):
                print("Dimension: "+str(dim)+" | Regional parameter: "+str(param)+" | Test num. "+str(test_num+1)+"\n")
                thr_test.append(threading.Thread(target=testPwl, args=(genType(dim,param), testName+"_"+str(dim)+"_"+str(param)+"_"+str(test_num+1), dim)))
                thr_test[-1].start()

        if regions > 2:
            regions = regions - 1

        if dim % MAXDIMENSIONSTHREADING == 0 or dim == MAXDIMENSION:
            finished = False
            finished_thr_test = [False]*len(thr_test)

            while not finished:
                for th in range(len(thr_test)):
                    if not thr_test[th].is_alive():
                        finished_thr_test[th] = True

                finished = True
                for th in range(len(finished_thr_test)):
                    if not finished_thr_test[th]:
                        finished = False

            thr_test = []

#   main   ############################################################################################################
#######################################################################################################################

summary = []

if tl_test:
    print("Sigle set truncated linear test."+"\n\n")
    dim = int(( len(tl_test) / 2 ) - 1)
    testTl(tl_test, "single_tl", dim)

elif pwl_test:
    print("Sigle set piecewise linear test."+"\n\n")
    dim = int(( len(pwl_test[0][0]) / 2 ) - 1)
    testPwl(pwl_test, "single_pwl", dim)

elif TEST_TYPE == 0:
    data_folder = "./tl/"
    os.system("mkdir "+data_folder)
    print("Random truncated linear tests."+"\n\n")
    testRunnerTl(generateTl, "tl")

elif TEST_TYPE == 1:
    data_folder = "./tl_norm/"
    os.system("mkdir "+data_folder)
    print("Normalized random linear tests."+"\n\n")
    testRunnerTl(generateTl_norm, "tl_norm")

elif TEST_TYPE == 2:
    data_folder = "./pwl_simp/"
    os.system("mkdir "+data_folder)
    print("Simple region piecewise linear tests."+"\n\n")
    testRunnerPwl(generatePwl_simp, "pwl_simp")

elif TEST_TYPE == 3:
    data_folder = "./pwl_cub/"
    os.system("mkdir "+data_folder)
    print("Cubic region piecewise linear tests."+"\n\n")
    testRunnerPwl2(generatePwl_cub, "pwl_cub")

else:
    print("There is no such test."+"\n\n")

if not (tl_test or pwl_test):
    createSummary()
