#!/usr/bin/python3.7

###########################################
######## SETTINGS #########################
###########################################

# Number of testing evaluations by function
NUMEVALUATIONS = 1000

# Number of significant decimal places
DECPRECISION = 7
DECPRECISION_form = ".7f"

# If "only_test" is set to non empty, such function will be the only one tested
# In this case, next settings are insignificant
# We do not check the validity of the entered data, use it with wisdom!
only_test = []

###########################################
# Some examples ###########################
###########################################
# only_test = [4, 9, 0, 1, 2, 3]
# only_test = [5, 6, 0, 1, -1, 2]
# only_test = [4, 3, -1, 1, 0, 1]

# If "only_test" is set to empty
# Number of random functions to be tested
NUMTESTS = 100

# Maximum dimension used in random functions
MAXDIMENSION = 50

# Maximum absolute value of random numerators and denumerators
MAXINTEGER = 100

###########################################
######## END OF SETTINGS ##################
###########################################

import random
import os
from subprocess import check_output
import sys

def test(t,coefficient):
    print("Test "+str(t))

    if coefficient:
        dim = int((len(coefficient)/2)-1)
    else:
        dim = random.randint(0,MAXDIMENSION)
        for c in range(dim+1):
            coefficient.append(random.randint(-MAXINTEGER,MAXINTEGER))
            coefficient.append(random.randint(1,MAXINTEGER))

    pwl_file = open("test"+str(t)+".pwl", "w")
    pwl_file.write("tl ")
    for c in range(len(coefficient)):
        pwl_file.write(str(coefficient[c]))
        if c != len(coefficient)-1:
            pwl_file.write(" ")
    pwl_file.close()

    os.system("~/pwl2limodsat/bin/Release/pwl2limodsat ~/pwl2limodsat/tests/auto/test"+str(t)+".pwl")

    out_file = open("test"+str(t)+".out", "r")
    formula = []
    maxvar = dim
    phi = False
    smt_aux = []

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

    results_file = open("test"+str(t)+".res", "w")

    for evalit in range(NUMEVALUATIONS):
        print("Evaluation num. "+str(evalit+1))

        smt_file = open("test"+str(t)+".smt", "w")

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

        for var in range(dim+1,maxvar+1):
            smt_file.write("(assert (>= X"+str(var)+" 0))"+"\n")
            smt_file.write("(assert (<= X"+str(var)+" 1))"+"\n")

        smt_file.write("\n")

        for string in smt_aux:
            smt_file.write(string+"\n")

        smt_file.write("\n")

        value = [1]
        for var in range(1,dim+1):
            value.append(random.randint(0,10**DECPRECISION)/10**DECPRECISION)
            smt_file.write("(assert (= X"+str(var)+" "+format(value[len(value)-1],DECPRECISION_form)+"))"+"\n")

        smt_file.write("\n")
        smt_file.write("(check-sat)")
        smt_file.write("\n")
        smt_file.write("(get-value (phi))")

        smt_file.close()

        evaluation = 0
        for val in range(0,2*(dim+1),2):
            evaluation += (coefficient[val]/coefficient[val+1])*value[int(val/2)]

        evaluation = min([1,max([0,evaluation])])

        smt_out = check_output(["yices-smt2","test"+str(t)+".smt"]).decode(sys.stdout.encoding)

        if smt_out[smt_out.find("phi")+4 : smt_out.find("phi")+6] != "(/":
            eval_modsat = int(smt_out[smt_out.find("phi")+4 : smt_out.find(")")])
        else:
            beginpos = smt_out.find("phi")+7
            endpos = beginpos+smt_out[beginpos:].find(" ")
            beginpos2 = endpos+1
            endpos2 = beginpos2+smt_out[beginpos2:].find(")")
            eval_modsat = int(smt_out[beginpos : endpos]) / int(smt_out[beginpos2 : endpos2])

        final_analysis = "Passed all tests :)"

        if abs(evaluation - eval_modsat) < 10**-DECPRECISION:
            analysis = "OK"
        else:
            analysis = ":("
            final_analysis = "Failed at least a test :((((((((((((((("

        results_file.write(analysis+" | eval: "+str(evaluation)+" | evalModSat: "+str(eval_modsat)+"\n")

    results_file.write("\n\n")
    results_file.write(final_analysis)

    results_file.close()

    os.system("rm "+"test"+str(t)+".smt")

    if summary_file:
        summary_file.write("Test "+str(t)+": "+final_analysis+"\n")

summary_file = []

if only_test:
    test(0,only_test)
else:
    summary_file = open("testSummary.res", "w")

    for t in range(1,NUMTESTS+1):
        test(t,[])

    summary_file.close()
