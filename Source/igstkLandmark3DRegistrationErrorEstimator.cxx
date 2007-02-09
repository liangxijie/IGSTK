/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationErrorEstimator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkLandmark3DRegistrationErrorEstimator_cxx
#define __igstkLandmark3DRegistrationErrorEstimator_cxx

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "igstkLandmark3DRegistrationErrorEstimator.h"

#include "itkMatrix.h"
#include "itkSymmetricEigenAnalysis.h"
#include "vnl/vnl_math.h"

namespace igstk
{ 
/** Constructor */
Landmark3DRegistrationErrorEstimator::
  Landmark3DRegistrationErrorEstimator() :  m_StateMachine( this )
{
  m_LandmarkRegistrationError = 0.0;

  // Set the state descriptors
  igstkAddStateMacro ( Idle );
  igstkAddStateMacro ( LandmarkContainerSet );
  igstkAddStateMacro ( LandmarkRegistrationErrorSet );
  igstkAddStateMacro ( AttemptingToComputeErrorParameters );
  igstkAddStateMacro ( ErrorParametersComputed );
  igstkAddStateMacro ( TargetPointSet );
  igstkAddStateMacro ( AttemptingToEstimateTargetRegstirationError );  
  igstkAddStateMacro ( TargetRegistrationErrorEstimated );
 
  // Set the Input descriptors 
  igstkAddInputMacro( LandmarkContainer );
  igstkAddInputMacro( LandmarkRegistrationError );
  igstkAddInputMacro( ComputeErrorParameters );
  igstkAddInputMacro( ErrorParametersComputationSuccess );
  igstkAddInputMacro( ErrorParametersComputationFailure );
  igstkAddInputMacro( TargetPoint ); 
  igstkAddInputMacro( EstimateTargetPointRegistrationError ); 
  igstkAddInputMacro( TargetPointRegistrationErrorEstimationSuccess ); 
  igstkAddInputMacro( TargetPointRegistrationErrorEstimationFailure ); 
  igstkAddInputMacro( GetTargetPointRegistrationErrorEstimate ); 

  // Add the transitions
 
  igstkAddTransitionMacro( Idle, 
                          LandmarkContainer,
                          LandmarkContainerSet,
                          SetLandmarkContainer );

  igstkAddTransitionMacro( LandmarkContainerSet,
                          LandmarkRegistrationError,
                          LandmarkRegistrationErrorSet, 
                          SetLandmarkRegistrationError );

  igstkAddTransitionMacro( LandmarkRegistrationErrorSet,
                          ComputeErrorParameters, 
                          AttemptingToComputeErrorParameters,
                          ComputeErrorParameters );

  igstkAddTransitionMacro( AttemptingToComputeErrorParameters,
                          ErrorParametersComputationSuccess,
                          ErrorParametersComputed,
                          ReportSuccessInErrorParametersComputation );

  igstkAddTransitionMacro( AttemptingToComputeErrorParameters,
                          ErrorParametersComputationFailure,
                          LandmarkRegistrationErrorSet,
                          ReportFailureInErrorParametersComputation );

  igstkAddTransitionMacro( ErrorParametersComputed,
                          TargetPoint,
                          TargetPointSet,
                          SetTargetPoint );

  igstkAddTransitionMacro( TargetPointSet,
                          EstimateTargetPointRegistrationError, 
                          AttemptingToEstimateTargetRegstirationError,
                          EstimateTargetPointRegistrationError );

  igstkAddTransitionMacro( AttemptingToEstimateTargetRegstirationError,
                          TargetPointRegistrationErrorEstimationSuccess,
                          TargetRegistrationErrorEstimated,
                     ReportSuccessInTargetPointRegistrationErrorEstimation );

  igstkAddTransitionMacro( AttemptingToEstimateTargetRegstirationError,
                          TargetPointRegistrationErrorEstimationFailure,
                          TargetPointSet,
                       ReportFailureInTargePointRegistrationErrorEstimation );
 
  igstkAddTransitionMacro( TargetRegistrationErrorEstimated, 
                          GetTargetPointRegistrationErrorEstimate,
                          TargetRegistrationErrorEstimated,
                          GetTargetPointRegistrationErrorEstimate );

  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

} 

/** Destructor */
Landmark3DRegistrationErrorEstimator::~Landmark3DRegistrationErrorEstimator()  
{

}

/** RequestSetLandmarkContainer method is used to set the landmark container */
void 
Landmark3DRegistrationErrorEstimator::RequestSetLandmarkContainer( 
                            const LandmarkContainerType & landmarkContainer )
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "RequestSetLandmarkContainer called...\n");
  this->m_LandmarkContainerToBeSet = landmarkContainer;
  igstkPushInputMacro( LandmarkContainer );
  this->m_StateMachine.ProcessInputs();
}

/** SetLandmarkContainer method is used by the state machine to 
 *  set the landmark container*/
void 
Landmark3DRegistrationErrorEstimator::SetLandmarkContainerProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "SetLandmarkContainerProcessing called...\n");
  this->m_LandmarkContainer= this->m_LandmarkContainerToBeSet; 
}

/** RequestSetLandmarkRegistrationError method is used to set 
 *  the landmark container */
void 
Landmark3DRegistrationErrorEstimator
::RequestSetLandmarkRegistrationError( const ErrorType & error )
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "RequestSetLandmarkRegistrationError called...\n");
  this->m_LandmarkRegistrationErrorToBeSet = error;
  igstkPushInputMacro( LandmarkRegistrationError );
  this->m_StateMachine.ProcessInputs();
}

/** SetLandmarkRegistrationErrorProcessing method is used 
 *  by the state machine to set the landmark registration error */
void 
Landmark3DRegistrationErrorEstimator::SetLandmarkRegistrationErrorProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "SetLandmarkRegistrationErrorProcessing called...\n");
  this->m_LandmarkRegistrationError = this->m_LandmarkRegistrationErrorToBeSet; 
}

/** RequestSetTargetPoint is used to set the target point location */
void 
Landmark3DRegistrationErrorEstimator
::RequestSetTargetPoint( const TargetPointType & targetpoint )
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "RequestSetTargetPoint called...\n");
  this->m_TargetPointToBeSet = targetpoint;
  igstkPushInputMacro( TargetPoint );
  this->m_StateMachine.ProcessInputs();
}

/** SetTargetPointProcessing method is used by the state machine to 
 *  set the target point*/
void 
Landmark3DRegistrationErrorEstimator::SetTargetPointProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "SetTargetPointProcessing called...\n");
  this->m_TargetPoint= this->m_TargetPointToBeSet; 
}


/** Compute and set landmark principal axes */
void Landmark3DRegistrationErrorEstimator::ComputeLandmarkPrincipalAxes()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "ComputeLandmarkPrincipalAxes called...\n");

  typedef itk::Matrix<double,3,3>                 MatrixType;
  typedef itk::Matrix<double,4,4>                 AugmentedMatrixType;
  typedef itk::Vector<double,3>                   VectorType;
  typedef itk::Vector<double,4>                   AugmentedVectorType;

  MatrixType                                      covarianceMatrix;
   
  PointsContainerConstIterator                    pointItr;
   
  VectorType                                      landmarkVector;
  VectorType                                      landmarkCentered;
  
  landmarkVector.Fill(0.0);
   
  pointItr  = m_LandmarkContainer.begin();
  while( pointItr != m_LandmarkContainer.end() )
    {
    landmarkVector[0] += (*pointItr)[0];
    landmarkVector[1] += (*pointItr)[1];
    landmarkVector[2] += (*pointItr)[2];
    ++pointItr;
    }

  pointItr  = m_LandmarkContainer.begin();
  while( pointItr != m_LandmarkContainer.end() )
    {
    for(unsigned int i=0; i<3; i++)
      {
      landmarkCentered[i]  = (*pointItr)[i]  - this->m_LandmarkCentroid[i];
      }

    for(unsigned int i=0; i<3; i++)
      {
      for(unsigned int j=0; j<3; j++)
        {
        covarianceMatrix[i][j] += landmarkCentered[i] * landmarkCentered[j];
        }
      }
    ++pointItr;
    }

  // Expand the matrix 
  AugmentedMatrixType              augmentedCovarianceMatrix;
        
  augmentedCovarianceMatrix[0][0] =  covarianceMatrix[0][0] 
                                     + covarianceMatrix[1][1] 
                                     + covarianceMatrix[2][2];
  augmentedCovarianceMatrix[1][1] =  covarianceMatrix[0][0] 
                                     - covarianceMatrix[1][1] 
                                     - covarianceMatrix[2][2];
  augmentedCovarianceMatrix[2][2] = -covarianceMatrix[0][0] 
                                     + covarianceMatrix[1][1] 
                                     - covarianceMatrix[2][2];
  augmentedCovarianceMatrix[3][3] = -covarianceMatrix[0][0] 
                                    - covarianceMatrix[1][1] 
                                    +covarianceMatrix[2][2];
        
  // off-diagonal elements
  augmentedCovarianceMatrix[0][1] = augmentedCovarianceMatrix[1][0] = 
                                    covarianceMatrix[1][2] 
                                    -covarianceMatrix[2][1];
  augmentedCovarianceMatrix[0][2] = augmentedCovarianceMatrix[2][0] = 
                                    covarianceMatrix[2][0] 
                                    -covarianceMatrix[0][2];
  augmentedCovarianceMatrix[0][3] = augmentedCovarianceMatrix[3][0] = 
                                    covarianceMatrix[0][1] 
                                    -covarianceMatrix[1][0];

  augmentedCovarianceMatrix[1][2] = augmentedCovarianceMatrix[2][1] = 
                                    covarianceMatrix[0][1] 
                                    +covarianceMatrix[1][0];
  augmentedCovarianceMatrix[1][3] = augmentedCovarianceMatrix[3][1] = 
                                    covarianceMatrix[2][0] 
                                    +covarianceMatrix[0][2];
  augmentedCovarianceMatrix[2][3] = augmentedCovarianceMatrix[3][2] = 
                                    covarianceMatrix[1][2] 
                                    +covarianceMatrix[2][1];
  
  AugmentedMatrixType                    eigenVectors;
  AugmentedVectorType                    eigenValues;

  typedef itk::SymmetricEigenAnalysis< 
          AugmentedMatrixType,  
          AugmentedVectorType,
          AugmentedMatrixType > SymmetricEigenAnalysisType;

  SymmetricEigenAnalysisType symmetricEigenSystem(4);
        
  symmetricEigenSystem.ComputeEigenValuesAndVectors
              ( augmentedCovarianceMatrix, eigenValues, eigenVectors );

  this->m_LandmarkPrincipalAxes.Set( eigenVectors[3][1],
                   eigenVectors[3][2],
                   eigenVectors[3][3],
                   eigenVectors[3][0]  );
}

/** Compute landmark centroid */
void Landmark3DRegistrationErrorEstimator::ComputeLandmarksCentroid()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "ComputeLandmarksCentroid called...\n");

  typedef itk::Vector< double, 3 >                      VectorType;

  PointsContainerConstIterator                          pointItr;
  VectorType                                            landmarkVector;

  // initialize to zero
  landmarkVector[0] = 0.0;
  landmarkVector[1] = 0.0;
  landmarkVector[2] = 0.0;

  pointItr  = m_LandmarkContainer.begin();
  while( pointItr != m_LandmarkContainer.end() )
    {
    landmarkVector[0] += (*pointItr)[0];
    landmarkVector[1] += (*pointItr)[1];
    landmarkVector[2] += (*pointItr)[2];
    ++pointItr;
    }

  for(unsigned int ic=0; ic<3; ic++)
    {
    this->m_LandmarkCentroid[ic]  = landmarkVector[ic]  / 
                                    this->m_LandmarkContainer.size();
    } 
}

/** Compute RMS distance of landmarks from the principal axes */
void Landmark3DRegistrationErrorEstimator::
     ComputeRMSDistanceLandmarksFromPrincipalAxes()
{
  
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "ComputeRMSDistanceLandmarksFromPrincipalAxes called...\n");

  typedef itk::Vector< double, 3 >                VectorType;
  typedef itk::Matrix< double, 3, 3 >             MatrixType; 
  typedef double                                  DistanceType; 
  
  PointsContainerConstIterator                    pointItr;
  VectorType                                      landmarkVector;
  MatrixType                                      principalAxesMatrix;
  VectorType                                      principalAxes1;
  VectorType                                      principalAxes2;
  VectorType                                      principalAxes3;
    
  // Convert versor to a matrix
  principalAxesMatrix = this->m_LandmarkPrincipalAxes.GetMatrix();
 
  for ( unsigned int i=0; i<3; i++ ) 
    {
    principalAxes1[i] = principalAxesMatrix[0][i]; 
    principalAxes2[i] = principalAxesMatrix[1][i]; 
    principalAxes3[i] = principalAxesMatrix[2][i]; 
    }
  
  // Normalize the principal axes 
  VectorType        normalizedPrincipalAxes1;
  VectorType        normalizedPrincipalAxes2;
  VectorType        normalizedPrincipalAxes3;

  normalizedPrincipalAxes1 = principalAxes1;
  normalizedPrincipalAxes2 = principalAxes1;
  normalizedPrincipalAxes3 = principalAxes1;

  normalizedPrincipalAxes1.Normalize();
  normalizedPrincipalAxes2.Normalize();
  normalizedPrincipalAxes3.Normalize();

  landmarkVector.Fill(0.0);
 
  // Vector from landmark point to landmark centroid  
  VectorType                                       differenceVector;
 
  //  Distance from the landmark point to the principal axes
  DistanceType    distanceFromLandmarkPointToPrinciaplAxes1 = 0.0;
  DistanceType    distanceFromLandmarkPointToPrinciaplAxes2 = 0.0;
  DistanceType    distanceFromLandmarkPointToPrinciaplAxes3 = 0.0;
  
  pointItr  = m_LandmarkContainer.begin();
  while( pointItr != m_LandmarkContainer.end() )
    {
    differenceVector[0] = (*pointItr)[0] - this->m_LandmarkCentroid[0];
    differenceVector[1] = (*pointItr)[1] - this->m_LandmarkCentroid[1];
    differenceVector[2] = (*pointItr)[2] - this->m_LandmarkCentroid[2];
   
    distanceFromLandmarkPointToPrinciaplAxes1 += 
        vnl_math_sqr( itk::CrossProduct ( normalizedPrincipalAxes1, 
                                          differenceVector ).GetNorm() ); 
    distanceFromLandmarkPointToPrinciaplAxes2 += 
        vnl_math_sqr( itk::CrossProduct ( normalizedPrincipalAxes2, 
                                          differenceVector ).GetNorm() ); 
    distanceFromLandmarkPointToPrinciaplAxes3 += 
        vnl_math_sqr( itk::CrossProduct ( normalizedPrincipalAxes3, 
                                          differenceVector ).GetNorm() ); 
    ++pointItr;
    }

  this->m_RMSDistanceFromLandmarkToPrincipalAxes[0] = 
        sqrt( distanceFromLandmarkPointToPrinciaplAxes1 / 
                                                 m_LandmarkContainer.size() );
  this->m_RMSDistanceFromLandmarkToPrincipalAxes[1] = 
        sqrt( distanceFromLandmarkPointToPrinciaplAxes2 /
                                                 m_LandmarkContainer.size() );
  this->m_RMSDistanceFromLandmarkToPrincipalAxes[2] = 
        sqrt( distanceFromLandmarkPointToPrinciaplAxes3 / 
                                                 m_LandmarkContainer.size() );
  
}

/** RequestComputeErrorParameters is a public method to request error 
 *  parameters computation */
void
Landmark3DRegistrationErrorEstimator::RequestComputeErrorParameters()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "RequestComputeErrorParameters called...\n");

  igstkPushInputMacro( ComputeErrorParameters );
  this->m_StateMachine.ProcessInputs();
}

/** RequestEstimateTargetPointRegistrationError is a public method 
 *  to request error parameters computation */
void
Landmark3DRegistrationErrorEstimator
::RequestEstimateTargetPointRegistrationError()
{

  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "RequestEstimateTargetPointRegistrationError called...\n");
 
  igstkPushInputMacro( EstimateTargetPointRegistrationError );
  this->m_StateMachine.ProcessInputs();
}


/** The "ComputeErrorParametersParamters" method calculates all the rigid body
 *  registration error parameters */
void 
Landmark3DRegistrationErrorEstimator:: ComputeErrorParametersProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "ComputeErrorParametersProcessing called...\n");

  this->ComputeLandmarksCentroid();
  this->ComputeLandmarkPrincipalAxes();
  this->ComputeRMSDistanceLandmarksFromPrincipalAxes();

  igstkPushInputMacro( ErrorParametersComputationSuccess );
  this->m_StateMachine.ProcessInputs();
}

/** The "EstimateTargetRegistrationError" method estimates 
 *  target signal ( sample point ) registration error */
void Landmark3DRegistrationErrorEstimator
::EstimateTargetPointRegistrationErrorProcessing( )
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "EstimateTargetRegistrationError called...\n");
  
  typedef itk::Vector< double, 3 >                VectorType;
  typedef itk::Matrix< double, 3, 3 >             MatrixType; 
  typedef double                                  DistanceType; 
  
  MatrixType                                      principalAxesMatrix;
  VectorType                                      principalAxes1;
  VectorType                                      principalAxes2;
  VectorType                                      principalAxes3;
  
  //  Distance vector from target point to the principal axes
  VectorType                           distanceFromTargetPointToPrincipalAxes;

  // Vector from the target point to the landmark centroid
  VectorType                           differenceVector;

  differenceVector = this->m_TargetPoint - this->m_LandmarkCentroid;

  // Convert versor to a matrix
  principalAxesMatrix = this->m_LandmarkPrincipalAxes.GetMatrix();
 
  for ( unsigned int i=0; i<3; i++ ) 
    {
    principalAxes1[i] = principalAxesMatrix[0][i]; 
    principalAxes2[i] = principalAxesMatrix[1][i]; 
    principalAxes3[i] = principalAxesMatrix[2][i]; 
    }
  
  // Normalize the principal axes 
  VectorType   normalizedPrincipalAxes1;
  VectorType   normalizedPrincipalAxes2;
  VectorType   normalizedPrincipalAxes3;

  normalizedPrincipalAxes1 = principalAxes1;
  normalizedPrincipalAxes2 = principalAxes1;
  normalizedPrincipalAxes3 = principalAxes1;

  normalizedPrincipalAxes1.Normalize();
  normalizedPrincipalAxes2.Normalize();
  normalizedPrincipalAxes3.Normalize();


  // calculate the distance from the target point to the principal axes
  distanceFromTargetPointToPrincipalAxes[0] = 
               itk::CrossProduct ( normalizedPrincipalAxes1, 
                                   differenceVector ).GetNorm(); 
  distanceFromTargetPointToPrincipalAxes[1] = 
               itk::CrossProduct ( normalizedPrincipalAxes2, 
                                   differenceVector ).GetNorm(); 
  distanceFromTargetPointToPrincipalAxes[2] = 
               itk::CrossProduct ( normalizedPrincipalAxes3, 
                                   differenceVector ).GetNorm(); 

  // target error parameter 
  ErrorType                      targetRegistrationError = 0.0;
 
  for( unsigned int i=0; i<3; i++ ) 
    {
    targetRegistrationError += 
           vnl_math_sqr( distanceFromTargetPointToPrincipalAxes[i] ) / 
           vnl_math_sqr( this->m_RMSDistanceFromLandmarkToPrincipalAxes[i] ); 
    }

  targetRegistrationError *= (1.0/3.0);
  targetRegistrationError += 1.0;
  targetRegistrationError *= ( vnl_math_sqr( 
                              this->m_LandmarkRegistrationError ) ) / 
                              ( this->m_LandmarkContainer.size() - 2 );

  this->m_TargetPointRegistrationError = targetRegistrationError;
  
  igstkPushInputMacro( TargetPointRegistrationErrorEstimationSuccess );
  this->m_StateMachine.ProcessInputs();
}

/** GetTargetPointRegistrationErrorEstimate method requests the target point 
 *  registration error */
void 
Landmark3DRegistrationErrorEstimator
::RequestGetTargetPointRegistrationErrorEstimate()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "GetTargetPointRegistrationErrorEstimate called...\n");
 
  igstkPushInputMacro( GetTargetPointRegistrationErrorEstimate );
  this->m_StateMachine.ProcessInputs();
}


/** GetTargetPointRegistrationError method throws an event loaded with the 
 *  landmark registration error */
void 
Landmark3DRegistrationErrorEstimator
::GetTargetPointRegistrationErrorEstimateProcessing()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
              "GetTargetPointRegistrationErrorEstimateProcessing called...\n");

  LandmarkRegistrationErrorEvent                event;
  event.Set( m_TargetPointRegistrationError );
  this->InvokeEvent( event );
}

/** The ReportFailureInErrorParametersComputation reports error in 
 *  error parameters computation */
void
Landmark3DRegistrationErrorEstimator
::ReportFailureInErrorParametersComputationProcessing( )
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator"
                "ReportFailureInErrorParametersComputation called ...\n" );
}

/** The ReportFailureInTargePointRegistrationErrorEstimationProcessing 
 *  reports success in error parameters computation */
void
Landmark3DRegistrationErrorEstimator
::ReportFailureInTargePointRegistrationErrorEstimationProcessing( )
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator"
        "ReportFailureInTargePointRegistrationErrorEstimation called ...\n" );
}


/** The ReportSuccessInErrorParametersComputationProcessing reports success
 *  in error paramter computation */
void
Landmark3DRegistrationErrorEstimator
::ReportSuccessInErrorParametersComputationProcessing( )
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator"
        "ReportSuccessInErrorParametersComputationProcessing called ...\n" );
}

/** The ReportSuccessInTargetPointRegistrationErrorEstimation reports 
 *  success in target point registration error compuation */
void
Landmark3DRegistrationErrorEstimator
::ReportSuccessInTargetPointRegistrationErrorEstimationProcessing( )
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator"
                "ReportSuccessInTargePointRegistrationErrorEstimation\
                Processing called ...\n" );
}

/** Print Self function */
void 
Landmark3DRegistrationErrorEstimator::PrintSelf( std::ostream& os,
                                             itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Image Landmarks: " << std::endl;
  PointsContainerConstIterator mitr = m_LandmarkContainer.begin();
  while( mitr != m_LandmarkContainer.end() )
    {
    os << indent << *mitr << std::endl;
    ++mitr;
    }
  os << indent << "LandmarkRegistrationError: "
     << m_LandmarkRegistrationError << std::endl;
  os << indent << "Landmark Principal Axes: " << std::endl;
  os << indent << this->m_LandmarkPrincipalAxes << std::endl;
}

} // end namespace igstk
#endif
