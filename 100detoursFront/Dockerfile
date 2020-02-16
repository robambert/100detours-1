FROM node:8
WORKDIR /usr/src/app
COPY package*.json ./

RUN npm install --silent
# Copy app source code
COPY . .

EXPOSE 3000
CMD ["npm", "start"]
