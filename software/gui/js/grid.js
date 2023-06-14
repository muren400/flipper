export class Grid {
    constructor() {
        this.tableBody = document.getElementById('dot_matrix_table_body');
        this.initGrid();

        const convertButton = document.getElementById('print_interger_array_button');
        convertButton.addEventListener('click', () => {
            this.convertIntoIntegerArray();
        });
    }

    initGrid() {
        for (let i = 0; i < 11; i++) {
            let row = document.createElement('tr');
            this.tableBody.appendChild(row);

            for (let j = 0; j < 5; j++) {
                let disc = document.createElement('td');
                disc.addEventListener('mousedown', () => {
                    this.flipDisc(disc);
                });

                disc.addEventListener('mouseenter', (e) => {
                    switch(e.buttons) {
                        case 1:
                            this.setDisc(disc, true);
                            break;

                        default:
                            break;
                    }
                });

                row.appendChild(disc);
            }
        }
    }

    flipDisc(disc) {
        disc.classList.toggle('flipped');
    }

    setDisc(disc, value) {
        if(value === true) {
            disc.classList.add('flipped');
        } else {
            disc.classList.remove('flipped');
        }
    }

    convertIntoIntegerArray() {
        const rows = [];
        for (let row of this.tableBody.children) {
            let rowInteger = 0;
            for (let i = 0; i < row.childElementCount; i++) {
                const disc = row.children[i];
                if (disc.classList.contains('flipped') === false) {
                    continue;
                }

                rowInteger |=  1 << (4-i);
            }
            rows.push(rowInteger);
        }

        let convertedAsText = '';

        rows.forEach((row) => {
            convertedAsText += '0b' + this.dec2bin(row) + ',\n';
        });

        document.getElementById('converted_text_area').value = convertedAsText;

        // console.log(rows.map((row) => this.dec2bin(row)));

        return rows;
    }

    dec2bin(dec) {
        return (dec >>> 0).toString(2).padStart(5, '0');
    }
}
