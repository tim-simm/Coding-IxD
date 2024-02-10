const app = Vue.createApp({
    data() {
        return {
            pageCounter: 0,
            currentQuestion: 0,
            socialMedia: 0,
            comparison: 0,
            opinion: 0,
            startX: 0,
            isSwipeLeft: false,
            isSwipeRight: false,
            cardVisible: true,
            currentPosition: 0,
            age: '',
            height: '',
            totalQuestions: 9,
            answer: 'NULL',
            questions: [
            {text: 'Social media and celebrities are an important source of attractiveness',id:1},
            {text: 'Have you ever felt pressure from social media to change your look?',id:2},
            {text: 'Do you compare your body with others on social media?',id:3},
            {text: 'Do you think the ability to change your appearance through photo filters or editing has an impact on how you see your own body?',id:4},
            {text: 'Is it because different environments (like the beach or the gym) influence your urge to compare?',id:5},
            {text: 'Do you often compare yourself with earlier versions of yourself?',id:6},
            {text: 'Do you often speak positively or negatively about your body to yourself?',id:7},
            {text: 'Do you think that the portrayal of the "ideal" body in the media affects your self-esteem?',id:8},
            {text: 'Do you believe that social norms influence your attitude toward your body?',id:9}
            ],
            persona: 'neutral',
            personaText: 'It is important that you remember that while beauty and attractiveness can be pleasant and nice, they cannot provide any insights into a person`s character, personality, lifestyle, or the kind of life and treatment they deserve."',
            personaAdvice: 'Be as you are and do not distract yourself from others. Be proud of what your body does for yo',
            result: 0,
            personas: [
                {key:'body neutral', value: ['000','100','010','001','200','020','002','102','011']},
                {key:'body positive', value: ['110','101','111','112','121','012','120','210','211']},
                {key:'body negative', value: ['212','202','022','122','221','220','201','021','222']}
            ],
            takeAways: [
                {key: '"It is important that you remember that while beauty and attractiveness can be pleasant and nice, they cannot provide any insights into a person`s character, personality, lifestyle, or the kind of life and treatment they deserve."', advice: 'Be as you are and do not distract yourself from others. Be proud of what your body does for you'},
                {key: '"It is important that you show off your imperfections and reshape the narrative around what`s beautiful, you have the power to shine a light on the unconscious biases we are conditioned to hold."', advice: 'Maybe think about Body Neutrality  and appreciate your body more for keeping you alive day in, and day out'},
                {key: '"It seems society`s unrealistic beauty and body ideals have the unfortunate effect of making you feel unworthy and insecure. "', advice: 'Think more about body Neutrality and say goodbye to ideals. Respect your body and be grateful for it'}
            ]
        }
    },
    methods: {
        loadData() {
                if (this.socialMedia > 0 & this.socialMedia <3) {this.socialMedia = 1;}
                else if (this.socialMedia == 3) {this.socialMedia = 2;}
                else {this.socialMedia = 0;}

                if (this.comparison > 0 & this.comparison < 3) {this.comparison = 1;}
                else if (this.comparison >= 3) {this.comparison = 2;}
                else {this.comparison = 0;}

                this.persona = String(this.socialMedia) + String(this.comparison) + String(this.opinion);

                if (this.personas[0].value.includes(this.persona)) {this.result=0; this.persona='Neutral'; console.log("0");}
                else if (this.personas[1].value.includes(this.persona)) {this.result=1; this.persona='Positive'; console.log("1");}
                else if (this.personas[2].value.includes(this.persona)) {this.result=2; this.persona='Negative'; console.log("2");}
                else {this.result = 0; console.log("default");}

                if (this.persona == 'Neutral') {this.personaText=this.takeAways[0].key; this.personaAdvice=this.takeAways[0].advice;}
                else if (this.persona == 'Positive') {this.personaText=this.takeAways[1].key; this.personaAdvice=this.takeAways[1].advice;}
                else if (this.persona == 'Negative') {this.personaText=this.takeAways[2].key; this.personaAdvice=this.takeAways[2].advice;}

                this.pageCounter = 2;
                this.saveOnDatabase();
        },

        saveOnDatabase() {
                const formData = new FormData();
                formData.append('age', this.age);
                formData.append('height', this.height);
                formData.append('result', this.result);

                console.log('age', this.age);
                console.log('height', this.height);
                console.log('result', this.result);

                const xhr = new XMLHttpRequest();
                const url = 'https://lamit03.userpage.fu-berlin.de/Questionnaire/server.php';

                xhr.open('POST', url, true);

                xhr.onreadystatechange = function () {
                    if (xhr.readyState === 4) {
                    if (xhr.status === 200) {
                        console.log('Data saved successfully');
                        // Hier können Sie entsprechend reagieren, wenn die Daten erfolgreich gespeichert wurden
                    } else {
                        console.error('Error saving to database:', xhr.statusText);
                        // Hier können Sie entsprechend reagieren, wenn ein Fehler auftritt
                    }
                    }
                };
                xhr.send(formData);

        },  
        showAdvice() {
            this.pageCounter = 3;
        },
        handleTouchStart(event) {
            this.startX = event.clientX || event.touches[0].clientX;
        },
        handleTouchMove(event) {
            if (this.startX) {
                let currentX = event.clientX || event.touches[0].clientX;
                this.currentPosition = currentX - this.startX;
            }
        },
        handleTouchEnd(event) {
            if (this.startX) {
                let endX = event.clientX || event.changedTouches[0].clientX;
                let deltaX = endX - this.startX;

                if (deltaX > 50) {
                    this.moveToNextQuestion();
                    this.isSwipeRight = true;
                    this.isSwipeLeft = false;
                    this.answer = "'YES'";
                    if ( this.currentQuestion <= 3 & this.currentQuestion != null) {
                        this.socialMedia += 1;
                    }
                    else if (this.currentQuestion <= 7 & this.currentQuestion > 3 & this.currentQuestion != null) {
                        this.comparison += 1;
                    }
                    else if (this.currentQuestion <= 9 & this.currentQuestion > 7) {
                        this.opinion += 1;
                    }
                } else if (deltaX < -50) {
                    this.moveToNextQuestion();
                    this.isSwipeLeft = true;
                    this.isSwipeRight = false;
                    this.answer = "'NO'";
                } else {
                    this.isSwipeLeft = false;
                    this.isSwipeRight = false;
                }

                this.startX = 0;
                this.currentPosition = 0;     
            }
        },
        moveToNextQuestion() {
            this.cardVisible = false;
            
            this.currentQuestion++;
            if (this.currentQuestion === this.questions.length) {
                // Alle Fragen beantwortet, keine weitere Frage anzeigen.
                setTimeout(() => {
                    this.isSwipeLeft = false;
                    this.isSwipeRight = false;
                    this.cardVisible = true;
                    this.pageCounter = 1;
                }, 400);
            }

            setTimeout(() => {
                this.isSwipeLeft = false;
                this.isSwipeRight = false;
                this.cardVisible = true;
            }, 400);
                  
        },

        showAndHide() {
            x = document.getElementById('sectionAdvice');
            if (x.style.display == 'none') {
                x.style.display = 'block';
            } else {
                x.style.display = 'none'
            }
        },
        reset() {
            window.location.href = 'https://lamit03.userpage.fu-berlin.de/Scale/';
        }
    }   

});


app.mount('#app');