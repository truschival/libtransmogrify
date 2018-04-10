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
        sh 'cmake -B build -H ~/src'
        sh 'cmake --build -B build'
      }
    }
  }
}