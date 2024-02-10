# Project Eigenform
## *Better understand your self-perception*

The project "Eigenform"  from [Coding IxD](https://codingixd.mi.fu-berlin.de/expandingthebody/) explored the quantitative and qualitative aspects of body self-image people encounter.

## Table of Contents
[Project Description](https://github.com/tim-simm/Coding-IxD/blob/main/README.md#project-description)

[Installation](https://github.com/tim-simm/Coding-IxD/blob/main/README.md#installation)

[Usage](https://github.com/tim-simm/Coding-IxD/blob/main/README.md#usage)

[Acknowledgments](https://github.com/tim-simm/Coding-IxD/blob/main/README.md#acknowledgments)

[License](https://github.com/tim-simm/Coding-IxD/blob/main/README.md#license)

## Project Description
### General Information
Every day, we come across the 'ideal' body, especially in the age of social media. The focus of such media tends to center around the perfect shape and form of bodies. Despite this emphasis, these idealized body images might fail to capture the diversity of people's bodies and sentiments. As a result, they contribute to a skewed self-perception based on universalized or generalized human bodies.

Motivated by this observation, Eigenform challenges conventional notions of self-perception by translating numeric values and emotional well-being into tangible forms. Rather than presenting precise values, the Eigenform object embodies tendencies without assigning judgments. It incorporates a carefully designed kinematic mechanism, physicalized in three individually expandable and contractible rings. Every user receives a unique representation of their body data, collected by a scale and our questionnaire. The rings' expansion and contraction metaphorically illustrate the potential gap between numerical data (like a weight on a scale) and people's body perception.

Eigenform serves as a medium for discussion to provoke reflection by teasing societal tendencies toward self-optimization. Ultimately, Eigenform reminds us how we should think about our bodies.

### Website
We created a website for the interaction and the questionnaire. This website guides the user through our project. The website has the following structure: 
At the beginning, a counter runs down, with which the interaction begins. The user stands on a scale and is weighed.
This is followed by explanations and an example of how the questionnaire is structured. 

The main interaction is the questionnaire. We have come up with a special design for the questionnaire, which is widely used and interesting. The questionnaire consists of yes/no questions and is answered by swiping to the left or right. 
In addition to the questions, the user has to indicate his age and height.

  ![RPReplay_Final1707562552](https://github.com/tim-simm/Coding-IxD/assets/130285233/16bf6c2d-0d8c-413c-8607-783db7277917)

A body type is then calculated using an algorithm. We differentiate between:
* Body Neutrality
* Body Positivity
* Body Negativity

The results are stored in an SQL database.

We worked with the Vue.js library for the interaction. The database queries with PHP, HTML for the pages and CSS for the style. The web application runs on a web server provided by the FU-Berlin, but it is also possible on every other server.

If you want to check it out: [Eigenform](https://lamit03.userpage.fu-berlin.de/Scale/)

### Arduino
We controlled the movement of three individual rings using stepper motors. To control these motors, we relied on the Arduino ESP32-feather model from Adafruit Huzzah. This model was suitable for our purpose because it allowed us to connect to WiFi and retrieve the data we collected with the questionnaire from the database. For more precise control of the motors, we also used a4988 stepper motor driver chips.

The code has been written with the Arduino IDE. Therefore, it consists of a setup() function and a loop() function. 
During the setup, we initialize the rings to their most retracted position by iterating through them and moving them by reading a separately installed button. Additionally, we fetch the most recent entry of the database to compare it against new entries. 

The main loop consists of several functions. In the beginning, we keep reading the scale value until we measure a certain threshold. Then, we read the value over 10 seconds and determine the weight, by calculating the average value. We then keep pulling the newest entry from the database, until it differs from the old entry, indicating that a user has filled out the questionnaire. Based on the calculated body image and the calculated BMI we assign one of nine individual Eigenform values to the user and move the rings accordingly. 

To reset the interaction, the rings return to their default position once the user steps off the scales. 

## Installation

## Usage

## Acknowledgments
*Our team consists of four computer science students and one product design student.*

Moritz Zitzow - Computer Science

Teoman Köse - Computer Science

Maarten Linnert - Computer Science

Tim Lamich - Computer Science 

Gustav Israel - Product Design 


## License
[License](https://github.com/tim-simm/Coding-IxD/blob/main/LICENSE)
