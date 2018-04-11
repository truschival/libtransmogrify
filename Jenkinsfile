pipeline {
  agent any
  stages {
    stage('Prepare') {
      steps {
        echo 'Running CMake'
        sh 'mkdir build'
      }
    }
    stage('Build') {
      steps {
        sh 'cmake -Bbuild -H. -DBUILD_TEST=On -DBUILD_GTEST_FROM_SRC=On'
        sh 'cmake --build build --target all'
      }
    }
  }
}