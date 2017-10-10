! Copyright (c) 2017 Copyright Tao Chiu All Rights Reserved.

SUBROUTINE ARRIVAL_MAIN(LO,LA,TIME_CUR,NUM_GRID) ! DON'T KNOW HOW TO USE COMMON PROPERLY
    ! USED MODULES
    USE LAYER_PARAMS
    IMPLICIT NONE

    ! PARAMETERS
    TYPE (LAYER) :: LO
    TYPE (LAYER),DIMENSION(NUM_GRID) :: LA
    INTEGER, INTENT(IN) :: NUM_GRID
    REAL, INTENT(IN) :: TIME_CUR

    ! LOCAL VARIABLES

    INTEGER :: I


    ! PROCESS ARRIVAL TIME
    CALL ARRIVAL( LO%Z(:,:,1), LO%NX, LO%NY, LO%ARRIVAL_TIME_MATRIX,TIME_CUR,LO%DT)
    if ( NUM_GRID .GT. 1 ) then
        do I = 1, NUM_GRID, 1
            if ( LA(I)%LAYSWITCH .EQ. 0 ) then
                CALL ARRIVAL( LA(I)%Z(:,:,1), LA(I)%NX, LA(I)%NY, LA(I)%ARRIVAL_TIME_MATRIX,TIME_CUR,LA(I)%DT)
            end if
        end do
    end if

END SUBROUTINE ARRIVAL_MAIN

SUBROUTINE ARRIVAL(LAYER,NX,NY,TIME,TIME_CUR,DT)
    IMPLICIT NONE

    !PARAMETERS
    INTEGER, INTENT(IN) :: NX, NY
    REAL, DIMENSION(NX, NY) , INTENT(IN) :: LAYER
    REAL, INTENT(IN) :: TIME_CUR
    REAL, INTENT(IN) :: DT
    REAL, DIMENSION(NX, NY, 2) , INTENT(OUT) :: TIME

    !LOCAL VARIABLES
    INTEGER :: I, J
    REAL :: RECORD_CRITERIA = 0.01
    REAL :: STEPS_CUR

    STEPS_CUR = TIME_CUR/DT + 1

    do J = 1, NY, 1
        do I = 1, NX, 1
            if ( TIME(I,J,1) .EQ. 0.0 ) then
                TIME(I,J,2) = ( TIME(I,J,2)*(STEPS_CUR - 1) + LAYER(I,J) )/STEPS_CUR ! AVG WAVE HEIGHT OVER TIME_SET(START) => TIME_CUR
                if ( ABS(LAYER(I,J) - TIME(I,J,2)) .GE. RECORD_CRITERIA ) then
                    TIME(I,J,1) = TIME_CUR
                end if
            end if
        end do
    end do

END SUBROUTINE ARRIVAL
