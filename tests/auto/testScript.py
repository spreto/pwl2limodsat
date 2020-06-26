#!/usr/bin/python3.7

import os
import random
import subprocess
import sys
import threading

#   settings   ########################################################################################################
#######################################################################################################################

yices = "yices-smt2"
data_folder = "./tl/"
pwl2limodsat_path = "../../bin/Release/pwl2limodsat"

# If tl_test is non empty, such function will be the only one tested
tl_test = []
# tl_test = [4, 9, 0, 1, 2, 3]
# tl_test = [5, 6, 0, 1, -1, 2]
# tl_test = [4, 3, -1, 1, 0, 1]

# If pwl_test is non empty and tl_test is empty, such function will be the only one tested
pwl_test = []

# If tl_test and pwl_test are empty, random tests of type TEST_TYPE will be performed
# 0: random truncated linear
TEST_TYPE = 0
MAXDIMENSIONTHREADING = 3

NUMEVALUATIONS = 100
DECPRECISION = 6
DECPRECISION_form = ".6f"

MAXDIMENSION = 9
NUMTESTSBYDIMENSION = 3
MAXINTEGER = 5

# MAXDIMENSION = 100
# NUMTESTSBYDIMENSION = 100
# MAXINTEGER = 100

#   creates pwl instance   ############################################################################################
#######################################################################################################################

def createTl(instance_data, pwl_file_name):
    pwl_file = open(data_folder+pwl_file_name, "w")
    pwl_file.write("tl ")
    for coeff in range(len(instance_data)):
        pwl_file.write(str(instance_data[coeff]))
        if coeff != len(instance_data)-1:
            pwl_file.write(" ")
    pwl_file.close()

#   creates limodsat instance   #######################################################################################
#######################################################################################################################

def pwl2out(pwl_file_name):
    os.system(pwl2limodsat_path+" "+data_folder+pwl_file_name)

#   generates random values   #########################################################################################
#######################################################################################################################

def generateValues(instance_dimension):
    propositional_variables_values = []

    for value in range(instance_dimension):
        propositional_variables_values.append(round(random.uniform(0,1), DECPRECISION))

    return propositional_variables_values

#   creates smt instance   ############################################################################################
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

#   compares values   #################################################################################################
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
        createSmt(test_file_name+".out", test_file_name+"_"+str(eval_num)+".smt", instance_dimension, values)

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

#   generates truncated linear function   #############################################################################
#######################################################################################################################

def generateTl(instance_dimension):
    instance_data = []

    for c in range(instance_dimension+1):
        instance_data.append(random.randint(-MAXINTEGER,MAXINTEGER))
        instance_data.append(random.randint(1,MAXINTEGER))

    return instance_data

#   creates summary   #################################################################################################
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

#   test   ############################################################################################################
#######################################################################################################################

def test(testType, genType, testName):
    thr_test = []

    for dim in range(MAXDIMENSION+1):
        for test_num in range(NUMTESTSBYDIMENSION):
            print("Dimension: "+str(dim)+" | Test num. "+str(test_num+1)+"\n")
            thr_test.append(threading.Thread(target=testType, args=(genType(dim), testName+"_"+str(dim)+"_"+str(test_num+1), dim)))
            thr_test[-1].start()

        if dim % MAXDIMENSIONTHREADING == 0 or dim == MAXDIMENSION:
            finished = False
            finished_thr_test = [False]*len(thr_test)

            while not finished:
                for th in range(len(thr_test)):
                    if not thr_test[th].isAlive():
                        finished_thr_test[th] = True

                finished = True
                for th in range(len(finished_thr_test)):
                    if not finished_thr_test[th]:
                        finished = False

            thr_test = []

#   main   ############################################################################################################
#######################################################################################################################

summary = []
os.system("mkdir "+data_folder)

if tl_test:
    print("Sigle set truncated linear test."+"\n\n")
    dim = int(( len(tl_test) / 2 ) - 1)
    testTl(tl_test, "single_tl", dim)

elif pwl_test:
    print("Sigle set piecewise linear test."+"\n\n")

elif TEST_TYPE == 0:
    print("Random truncated linear tests."+"\n\n")
    test(testTl, generateTl, "tl")

else:
    print("There is no such test."+"\n\n")

if not (tl_test or pwl_test):
    createSummary()
