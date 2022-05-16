# Orpheus
![alt text](https://github.com/chrigervi/Orpheus/blob/master/AdditionalResources/orpheus-logo.png?raw=true)

Orpheus is an audio plug-in based on the JUCE-Framework. Its designed to support artists who want to share their live cooperation over the internet with a broad audience.

### How to run Orpheus

Inside of the [bin](https://github.com/chrigervi/Orpheus/blob/master/bin/) folder you can find some pre-compiled binaries for Orpheus. Unfortunately, I can't provide binaries for every platform and/or plugin format. You'll need to compile the code by yourself in those cases.

### How is Orpheus structured?

This graph shows the component-structure of the Orpheus project.
The functionality can be split up into three different sections:
  1. The audio component - Dealing with the input, output and synchronisation
  2. The network component - Providing a safe and relient audio-stream
  3. The GUI component - Dealing with the graphical interface of the plugin

<img src="https://github.com/chrigervi/Orpheus/blob/master/AdditionalResources/orpheus-components-graph.jpg" width="50%">

### How to build the application by yourself

In order to successfully build Orpheus from the source code, you'll need [Projucer from the JUCE-Project](https://juce.com/discover/projucer). Open the ".jucer"-File of the Client or Server package with Projucer and select an development environment where you want to build the project. Projucer will generate a valid project for this environment.
