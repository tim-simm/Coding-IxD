const app = Vue.createApp({
    data() {
        return {
            currentQuestion: 0,
            selectedOption: 1,
            score: 0,
            startX: 0,
            isSwipeLeft: false,
            isSwipeRight: false,
            cardVisible: true,
            currentPosition: 0,
            currentQuestionIndex: 0,
            age: '',
            height: '',
            questions: [
            {
                text: 'How often do you compare your body to people on socialmedia?',
            },
            {
                text: 'How often do photo filters or edits affect how you see your own body?',
            },
            {
                text: 'How often do you compare your body to others?',
            },
            {
                text: 'Different settings (like the beach or gym) impact your urge to compare?',
            },
            {
                text: 'My overall fitness level is high.',
            },
            {
                text: 'Do you often compare yourself to past versions of yourself?',
            },
            {
                text: 'Do you often speak positively or negatively about your body to yourself?',
            },
            {
                text: 'Do you think the media’s portrayal of “ideal” bodies affects your self-esteem?',
            },
            {
                text: 'Being around good-looking people makes me feel bad about my body.',
            },
            {
                text: 'Do you believe societal standards impact your attitude towards your body?',
            }
            // Füge hier weitere Fragen hinzu
            ]
        }
    },
    computed: {
        currentQuestion() {
            return this.questions[this.currentQuestionIndex];
        }
    },
    methods: {
        saveOnDatabase() {
            const formData = new FormData();
            formData.append('age', this.age);
            formData.append('height', this.height);
            formData.append('score', this.score);

            console.log('age', this.age);
            console.log('height', this.height);
            console.log('score', this.score);

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
                    this.score += 1;
                } else if (deltaX < -50) {
                    this.moveToNextQuestion();
                    this.isSwipeLeft = true;
                    this.isSwipeRight = false;
                    this.score += 0;
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
                this.currentQuestion = null;
            }

            setTimeout(() => {
                this.isSwipeLeft = false;
                this.isSwipeRight = false;
                this.cardVisible = true;
            }, 400);
                  
        }
    }   
});


app.mount('#app');