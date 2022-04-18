# Orpheus
Orpheus is an audio plug-in based on the JUCE-Framework. Its designed to support artists who want to share their live cooperation over the internet with a broad audience.

## How is Orpheus structured?

This graph shows the component-structure of the Orpheus project.
The functionality can be split up into three different sections:
  1. The audio component - Dealing with the input, output and synchronisation
  2. The network component - Providing a safe and relient audio-stream
  3. The GUI component - Dealing with the graphical interface of the plugin

![alt text](https://github.com/chrigervi/Orpheus/blob/master/AdditionalResources/orpheus-components-graph.png?raw=true)

## How to build the application

In order to successfully build Orpheus from the source code, you'll need [Projucer from the JUCE-Project](https://juce.com/discover/projucer). Open the ".jucer"-Files of the Client or Server package with Projucer and select an development environment where to build the project. Projucer will generate an valid project for this environment. 
