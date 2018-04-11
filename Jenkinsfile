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
        sh 'cmake -Bbuild -H.'
        sh 'cmake --build build --target all'
      }
    }
  }
}