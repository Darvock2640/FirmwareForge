# Contributing to FirmwareForge
Thank you for your interest in contributing! Your help is greatly appreciated. This guide outlines the workflow and best practices for contributing to this repository.

## Table of Contents
- [Contributing to FirmwareForge](#contributing-to-firmwareforge)
  - [Table of Contents](#table-of-contents)
  - [Getting Started](#getting-started)
  - [Workflow](#workflow)
  - [Commit Messages](#commit-messages)
  - [Pull Requests](#pull-requests)
  - [Code Style](#code-style)

## Getting Started
1. Fork the Repository: Start by forking the main repository to your own GitHub account.

2. Clone Your Fork: Clone your forked repository to your local machine:

    ``` 
    git clone [https://github.com/your-username/your-repository-name.git](https://github.com/your-username/your-repository-name.git)

    cd your-repository-name

    ```


3. Create a New Branch: Always work on a new branch for each new feature or bug fix.

    ``` 
    git checkout -b feat/your-new-feature

    ```

## Workflow
Our contribution workflow is based on a simplified Gitflow model:

* The main branch is always stable and production-ready.

* All new features and bug fixes are developed in separate, dedicated branches.

* Changes are submitted via a Pull Request (PR) and merged into main after review.

## Commit Messages
We encourage using clear and descriptive commit messages. A good format is:

```
[type]: [description]

```


```type```: ```feat``` (new feature), ```fix``` (bug fix), ```docs``` `(documentation), ```style``` (formatting), ```refactor``` (code change that doesn't fix a bug or add a feature), ```test``` (adding tests), ```chore``` (build process or auxiliary changes).

description: A concise description of the change in 50 characters or less.

Example:
```
feat: Add new driver for BME280 sensor

```

## Pull Requests
When your changes are ready, push your branch to your forked repository and open a Pull Request against the main branch of the original repository.

* Fill out the template: Use the provided PR template to describe your changes, link to any related issues, and complete the checklist.

* Request a review: Once your PR is ready, request a review from the repository maintainer.

* Address feedback: Be prepared to discuss and refine your changes based on the review.

## Code Style
* Use clear, well-structured code with descriptive variable names.

* Add comments to explain complex logic, especially in header files.

* Ensure your code is portable and hardware-agnostic where possible.

* Avoid global variables unless absolutely necessary.